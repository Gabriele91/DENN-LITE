#include "DennConvolutional.h"
#define _metadata(_x, _default) (_x < metadata.size() ? metadata[_x] : _default)
namespace Denn
{
	///////////////////////////////////////	
	Convolutional::Convolutional
	(
	  ActivationFunction active_function
	, int in_width, int in_height, int in_channels
	, int window_width, int window_height, int out_channels
	)
	: Layer("convolutional",
		    { in_width, in_height, in_channels }, 
			{ (in_width - window_width + 1) , (in_height - window_height + 1), out_channels })
	, m_activation_function(active_function)
	, m_dim(in_channels, out_channels, in_height, in_width, window_height, window_width)
	{
		// Set data dimension
		const int filter_data_size = m_dim.in_channels * m_dim.out_channels * m_dim.filter_rows * m_dim.filter_cols;
		m_filter_data.resize(filter_data_size, 1);
		m_df_data.resize(filter_data_size);
		// Bias term
		m_bias.resize(m_dim.out_channels, 1);
		m_db.resize(m_dim.out_channels);
	}
	Convolutional::Convolutional
	(
		  const ActivationFunctions& active_function
		, const Shape& in
		, const std::vector<int>& metadata
	)
	: Convolutional
	(
	  active_function[0]
	//shapes
	, in.width(), in.height(), in.channels()
	//inputs
	, metadata[0], metadata[1], _metadata(2, 1)
	)
	{
	}
	//////////////////////////////////////////////////
	void Convolutional::activations(ActivationFunctions active_function)
	{
		m_activation_function = active_function[0];
	}
	ActivationFunctions Convolutional::activations() const
	{
		return { m_activation_function };
	}
	//////////////////////////////////////////////////
	const Inputs Convolutional::inputs() const
	{
		return { m_dim.filter_cols, m_dim.filter_rows, m_dim.out_channels };
	}
	//////////////////////////////////////////////////
	Layer::SPtr Convolutional::copy() const
	{
		return std::static_pointer_cast<Layer>(std::make_shared<Convolutional>(*this));
	}

	//////////////////////////////////////////////////
	const Matrix& Convolutional::feedforward(const Matrix& prev_layer_data)
	{
		// Each column is an observation
		const int nobs = prev_layer_data.cols();

		// Linear term, z = conv(in, w) + b
		m_z.resize(int(out_size()), nobs);

		// Convolution
		internal::convolve_valid(m_dim, prev_layer_data.data(), true, nobs,
			m_filter_data.data(), m_z.data()
		);

		// Add bias terms
		// Each column of m_z contains m_dim.out_channels channels, and each channel has
		// m_dim.conv_rows * m_dim.conv_cols elements
		int channel_start_row = 0;
		const int channel_nelem = m_dim.conv_rows * m_dim.conv_cols;
		for (int i = 0; i < m_dim.out_channels; i++, channel_start_row += channel_nelem)
		{
			m_z.block(channel_start_row, 0, channel_nelem, nobs).array() += m_bias(i,0);
		}

		// Apply activation function
		m_a.resize(int(out_size()), nobs);

		// Active
		m_activation_function.activate(m_z, m_a);

		return m_a;
	}
	const Matrix&  Convolutional::backpropagate(const Matrix& prev_layer_data, const Matrix& next_layer_data)
	{
		const int nobs = prev_layer_data.cols();

		// After forward stage, m_z contains z = conv(in, w) + b
		// Now we need to calculate d(L) / d(z) = [d(a) / d(z)] * [d(L) / d(a)]
		// d(L) / d(a) is computed in the next layer, contained in next_layer_data
		// The Jacobian matrix J = d(a) / d(z) is determined by the activation function
		Matrix& dLz = m_z;
		m_activation_function.jacobian(m_z, m_a, next_layer_data, dLz);

		// z_j = sum_i(conv(in_i, w_ij)) + b_j
		//
		// d(z_k) / d(w_ij) = 0, if k != j
		// d(L) / d(w_ij) = [d(z_j) / d(w_ij)] * [d(L) / d(z_j)] = sum_i{ [d(z_j) / d(w_ij)] * [d(L) / d(z_j)] }
		// = sum_i(conv(in_i, d(L) / d(z_j)))
		//
		// z_j is an image (matrix), b_j is a scalar
		// d(z_j) / d(b_j) = a matrix of the same size of d(z_j) filled with 1
		// d(L) / d(b_j) = (d(L) / d(z_j)).sum()
		//
		// d(z_j) / d(in_i) = conv_full_op(w_ij_rotate)
		// d(L) / d(in_i) = sum_j((d(z_j) / d(in_i)) * (d(L) / d(z_j))) = sum_j(conv_full(d(L) / d(z_j), w_ij_rotate))

		// Derivative for weights
		internal::ConvDims back_conv_dim(
			nobs, m_dim.out_channels,
			m_dim.channel_rows, m_dim.channel_cols,
			m_dim.conv_rows, m_dim.conv_cols
		);
		internal::convolve_valid(
			back_conv_dim, prev_layer_data.data(), false, m_dim.in_channels,
			dLz.data(), m_df_data.data()
		);
		m_df_data /= nobs;

		// Derivative for bias
		// Aggregate d(L) / d(z) in each output channel
		Matrix::ConstAlignedMapType dLz_by_channel(dLz.data(), m_dim.conv_rows * m_dim.conv_cols, m_dim.out_channels * nobs);
		ColVector dLb = dLz_by_channel.colwise().sum();
		// Average over observations
		Matrix::ConstAlignedMapType dLb_by_obs(dLb.data(), m_dim.out_channels, nobs);
		m_db.noalias() = dLb_by_obs.rowwise().mean();

		// Compute d(L) / d_in = conv_full(d(L) / d(z), w_rotate)
		m_din.resize(int(in_size()), nobs);
		internal::ConvDims conv_full_dim(m_dim.out_channels, m_dim.in_channels, m_dim.conv_rows, m_dim.conv_cols, m_dim.filter_rows, m_dim.filter_cols);
		internal::convolve_full(
			conv_full_dim, dLz.data(), nobs,
			m_filter_data.data(), m_din.data()
		);

		//return gradient
		return m_din;
	}
	void Convolutional::update(const Optimizer& optimize)
	{
		ColVector::ConstAlignedMapType dw(m_df_data.data(), m_df_data.size());
		ColVector::ConstAlignedMapType db(m_db.data(), m_db.size());
		ColVector::AlignedMapType      w(m_filter_data.data(), m_filter_data.size());
		ColVector::AlignedMapType      b(m_bias.data(), m_bias.size());

		optimize.update(dw, w);
		optimize.update(db, b);
	}
	//////////////////////////////////////////////////
	const Matrix& Convolutional::ff_output()
	{
		return m_a;
	}
	const Matrix& Convolutional::bp_output()
	{
		return m_din;
	}
	//////////////////////////////////////////////////
	size_t Convolutional::size() const
	{
		return 2;
	}
	Matrix& Convolutional::operator[](size_t i)
	{
		denn_assert(i < 2);
		switch (i)
		{
			default:
			case 0: return  m_filter_data;
			case 1: return  m_bias;
		}
	}
	const Matrix& Convolutional::operator[](size_t i) const
	{
		denn_assert(i < 2);
		switch (i)
		{
			default:
			case 0: return  m_filter_data;
			case 1: return  m_bias;
		}
	}
	//////////////////////////////////////////////////
}
