//String
namespace Denn
{
	template <typename T, typename Compare>
	inline std::vector<size_t> sort_permutation( const std::vector<T>& vec, Compare compare)
	{
		std::vector<size_t> p(vec.size());
		std::iota(p.begin(), p.end(), 0);
		std::sort(p.begin(), p.end(), [&](size_t i, size_t j) -> bool { return compare(vec[i], vec[j]); });
		return p;
	}

	template <typename T>
	inline std::vector<T> apply_permutation(const std::vector<T>& vec, const std::vector<std::size_t>& p)
	{
		std::vector<T> sorted_vec(vec.size());
		std::transform(p.begin(), p.end(), sorted_vec.begin(), [&](size_t i){ return vec[i]; });
		return sorted_vec;
	}

	template<typename T>
	inline void apply_permutation_in_place(std::vector<T>& v,  std::vector<size_t>& p)
	{	
		for (size_t i = 0; i < p.size(); i++) 
		{
			auto current = i;
			while (i != p[current])
			{
				auto next = p[current];
				std::swap(v[current], v[next]);
				p[current] = current;
				current = next;
			}
			p[current] = current;
		}
	}

	template<typename T>
	inline std::vector< std::vector< T > > multi_split(std::vector< T >& vin, std::vector< size_t > idxs)
	{
		if (idxs.size())
		{
			//push back last
			idxs.push_back(0);
			idxs.push_back(vin.size());
			//filter
			std::sort(idxs.begin(), idxs.begin());
			std::set<size_t> set_idxs(idxs.begin(), idxs.end());
			//split
			std::vector< std::vector< T > > out;
			auto first = set_idxs.begin();
			auto second = ++set_idxs.begin();

			while (second != set_idxs.end())
			{
				//test
				if (*first >= vin.size()) break;
				if (*second > vin.size()) break;
				//copy
				out.push_back(std::vector< T >(vin.begin() + *first, vin.begin() + *second));
				//next
				first = second;
				++second;
			}

			return out;
		}
		return {};
	}


	template<typename T>
	inline std::tuple< std::vector< T >, std::vector< T > > split(std::vector< T > vin, size_t idx)
	{
		//split 1
		if (!idx)
			return std::make_tuple< std::vector< T >, std::vector< T > >
			(
				std::vector< T >{},
				std::move(vin)
			);
		else if (vin.size() <= idx)
			return std::make_tuple< std::vector< T >, std::vector< T > >
			(
				std::move(vin),
				std::vector< T >{}
			);
		else
			return std::make_tuple< std::vector< T >, std::vector< T > >
			(
				std::vector< T >{ vin.begin(), vin.begin() + idx },
				std::vector< T >{ vin.begin() + idx, vin.end() }
			);
	}
}