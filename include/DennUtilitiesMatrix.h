//Matrix
namespace Denn
{
	/// Shifts a matrix/vector row-wise.
	/// A negative \a down value is taken to mean shifting up.
	/// When passed zero for \a down, the input matrix is returned unchanged.
	/// The type \a Matrix can be either a fixed- or dynamically-sized matrix.
	template < typename Matrix >
	inline Matrix shift_by_rows(const Matrix& in, typename Matrix::Index down)
	{
		//no swift
		if (!down) return in;
		//result
		Matrix out(in.rows(), in.cols());
		//mod
		if (down > 0) down = down % in.rows();
		else down = in.rows() - (-down % in.rows());
		int rest = in.rows() - down;
		//shif
		out.topRows(down)	 = in.bottomRows(down);
		out.bottomRows(rest) = in.topRows(rest);
		//return
		return out;
	}

	template < typename Matrix >
	inline Matrix shift_bottom(const Matrix& in, unsigned int down)
	{
		return shift_by_rows(in, int(down));
	}

	template < typename Matrix >
	inline Matrix shift_top(const Matrix& in, unsigned int top)
	{
		return shift_by_rows(in, -1*int(top));
	}
	
	template < typename Matrix >
	inline bool append_rows(Matrix& matrix, const Matrix& rows_to_append)
	{
		if (matrix.cols() != rows_to_append.cols()) return false;
		//alloc
		matrix.conservativeResize(matrix.rows() + rows_to_append.rows(), matrix.cols());
		//copy
		matrix.bottomRows(rows_to_append.rows()) = rows_to_append;
		//return
		return true;
	}

	template < typename Matrix >
	inline typename Matrix::Scalar distance_pow2(const Matrix& a, const Matrix& b)
	{
		return (a.array() - b.array()).square().sum();
	} 

	template < typename Matrix >
	inline typename Matrix::Scalar distance(const Matrix& a, const Matrix& b)
	{
		return std::sqrt(distance_pow2(a,b));
	}

	template < typename Matrix >
	void sort_ascending(Matrix& m) 
	{
  	std::sort(m.derived().data(), m.derived().data() + m.derived().size());
	}

	template < typename Matrix >
	void sort_rows_ascending(Matrix& m)
	{
		m.transposeInPlace();
		for (size_t r = 0; r < m.cols(); ++r)
			std::sort(m.col(r).data(), m.col(r).data() + m.col(r).size());
		m.transposeInPlace();
	}

	template < typename Matrix >
	void sort_descending(Matrix& m) 
	{
  	std::sort(m.derived().data(), m.derived().data() + m.derived().size());
		std::reverse(m.derived().data(), m.derived().data() + m.derived().size());
	}

	template < typename Matrix >
	void sort_rows_descending(Matrix& m)
	{
		m.transposeInPlace();
		for (typename Matrix::Index r = 0; r < m.cols(); ++r)
		{
			std::sort(m.col(r).data(), m.col(r).data() + m.col(r).size());
			std::reverse(m.col(r).data(), m.col(r).data() + m.col(r).size());
		}
		m.transposeInPlace();
	}

	template < typename Matrix >
	Matrix conv2d(const Matrix& input, const Matrix& kernel )
	{
		//output
		Matrix output = Matrix::Zero(input.rows(),input.cols());

		//alias
		using Index = typename Matrix::Index;
		using Scalar = typename Matrix::Scalar;

		//acc
		Scalar scalar;
		Scalar scalar2;

		//kenrel info
		Index col=0,row=0;
		Index k_size_x = kernel.rows();
		Index k_size_y = kernel.cols();

		//input lims
		Index limit_row = input.rows()-k_size_x;
		Index limit_col = input.cols()-k_size_y;

		#define NORMALIZE_CONV2
		#ifdef NORMALIZE_CONV2
			//normalization factor
			Scalar normalization = kernel.sum();
			if ( normalization <  std::numeric_limits<Scalar>::epsilon() ) normalization=1;
		#endif

		//apply kernel
		for (Index row = k_size_x; row < limit_row; row++ )
		for (Index col = k_size_y; col < limit_col; col++ )
		{
			Scalar b=(static_cast<Matrix>( input.block(row,col,k_size_x,k_size_y) ).cwiseProduct(kernel)).sum();
			output.coeffRef(row,col) = b;
		}
		
		#ifdef NORMALIZE_CONV2
			//normalize
			return output/normalization;
		#else
			return output;
		#endif 
	}

}
