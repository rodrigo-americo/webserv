/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   container.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bruno-valero <bruno-valero@student.42.f    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/20 11:32:29 by bruno-valer       #+#    #+#             */
/*   Updated: 2026/05/23 15:44:44 by bruno-valer      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONTAINER_HPP
# define CONTAINER_HPP

# include <map>
# include <unordered_map>
# include <set>
# include <unordered_set>

# include <vector>
# include <deque>
# include <list>
# include <queue>
# include <stack>

# include <functional>
# include <type_traits>
# include <algorithm>

namespace structure
{


	namespace detail
	{
		template <template <typename...> class ContainerType, typename... Args>
		class container_sequential;

		template <template <typename...> class ContainerType, typename... Args>
		class container_base
		{
			public:
				using type				=	ContainerType<Args...>;
				using value_type		=	typename type::value_type;
				using iterator			=	typename type::iterator;
				using const_iterator	=	typename type::const_iterator;

			protected:
				type						_container;

			public:
				container_base(): _container() {};
				~container_base() = default;

				const type								&getContainer() const { return _container; }
				type									&getContainer() { return _container; }

				const_iterator								begin() const { return _container.begin(); }
				iterator									begin() { return _container.begin(); }
				const_iterator								end() const { return _container.end(); }
				iterator									end() { return _container.end(); }

				size_t										size() const { return _container.size(); }
				bool										empty() const { return _container.empty(); }
				void										clear() { return _container.clear(); }

				void									forEach(std::function<void(const value_type &, bool &break_loop)> callback) const
				{
					bool break_loop = false;
					for (const_iterator it = _container.begin(); it != _container.end(); ++it)
					{
						callback(*it, break_loop);
						if (break_loop) break;
					}
				}

				container_sequential<std::vector, const_iterator>	filter(std::function<bool(const value_type &)> callback) const
				{
					container_sequential<std::vector, const_iterator>	filtered;
					filtered.getContainer().reserve(_container.size());
					for (const_iterator it = _container.begin(); it != _container.end(); ++it)
						if (callback(*it))
							filtered.getContainer().push_back(it);
					return filtered;
				}

				container_sequential<std::vector, iterator>			filter(std::function<bool(const value_type &)> callback)
				{
					container_sequential<std::vector, iterator>	filtered;
					filtered.getContainer().reserve(_container.size());
					for (iterator it = _container.begin(); it != _container.end(); ++it)
						if (callback(*it))
							filtered.getContainer().push_back(it);
					return filtered;
				}

				template <typename CallbackReturn>
				std::vector<CallbackReturn>							map(std::function<CallbackReturn(const value_type &)> callback) const
				{
					std::vector<CallbackReturn>	mapped;
					mapped.reserve(_container.size());
					for (const_iterator it = _container.begin(); it != _container.end(); ++it)
						mapped.push_back(callback(*it));
					return mapped;
				}

				template <typename Acc, typename Callback>
				Acc													reduce(Callback callback, Acc acc) const
				{
					for (const_iterator it = _container.begin(); it != _container.end(); ++it)
						callback(*it, acc);
					return acc;
				}

				template <typename Acc, typename Callback>
				void												reduce_ref(Callback callback, Acc &acc) const
				{
					for (const_iterator it = _container.begin(); it != _container.end(); ++it)
						callback(*it, acc);
				}

				const value_type									*find_if(std::function<bool(const value_type &)> callback) const
				{
					const_iterator	it = std::find_if(_container.begin(), _container.end(), callback);
					if (it == _container.end()) return nullptr;
					return &(*it);
				}

				value_type											*find_if(std::function<bool(const value_type &)> callback)
				{
					iterator	it = std::find_if(_container.begin(), _container.end(), callback);
					if (it == _container.end()) return nullptr;
					return &(*it);
				}

				bool												exists(std::function<bool(const value_type &)> callback) const
				{
					value_type	*item = find_if(callback);
					return !!item;
				}
		};

		template <template <typename...> class ContainerType, typename... Args>
		class container_sequential: public container_base<ContainerType, Args...>
		{
				using base = container_base<ContainerType, Args...>;
			public:
				using type				=	typename base::type;
				using value_type		=	typename type::value_type;
				using iterator			=	typename type::iterator;
				using const_iterator	=	typename type::const_iterator;

				container_sequential(): container_base<ContainerType, Args...>() {};
				~container_sequential() = default;

				const value_type	*find(const value_type &value) const
				{
					const_iterator	it = std::find(this->begin(), this->end(), value);
					if (it == this->end()) return nullptr;
					return &(*it);
				}

				value_type	*find(const value_type &value)
				{
					iterator	it = std::find(this->begin(), this->end(), value);
					if (it == this->end()) return nullptr;
					return &(*it);
				}

				bool		exists(const value_type &value) const
				{
					return !!find(value);
				}
		};

		template <template <typename...> class ContainerType, typename... Args>
		class container_associative: public container_base<ContainerType, Args...>
		{
				using base = container_base<ContainerType, Args...>;
			public:
				using type				=	typename base::type;
				using value_type		=	typename type::value_type;
				using iterator			=	typename type::iterator;
				using const_iterator	=	typename type::const_iterator;
				using key_type			=	typename type::key_type;

			public:
				container_associative(): container_base<ContainerType, Args...>() {};
				~container_associative() = default;

				const value_type	*find(const key_type &value) const
				{
					const_iterator	it = this->getContainer().find(value);
					if (it == this->end()) return nullptr;
					return &(*it);
				}

				value_type	*find(const key_type &value)
				{
					iterator	it = this->getContainer().find(value);
					if (it == this->end()) return nullptr;
					return &(*it);
				}

				bool		exists(const key_type &value) const
				{
					return !!find(value);
				}
		};
	}



	template <typename ...Args>
	using map = detail::container_associative<std::map, Args...>;
	template <typename ...Args>
	using unordered_map = detail::container_associative<std::unordered_map, Args...>;
	template <typename ...Args>
	using set = detail::container_associative<std::set, Args...>;
	template <typename ...Args>
	using unordered_set = detail::container_associative<std::unordered_set, Args...>;

	template <typename ...Args>
	using vector =  detail::container_sequential<std::vector, Args...>;
	template <typename ...Args>
	using deque =  detail::container_sequential<std::deque, Args...>;
	template <typename ...Args>
	using list =  detail::container_sequential<std::list, Args...>;
	template <typename ...Args>
	using queue =  detail::container_sequential<std::queue, Args...>;
	template <typename ...Args>
	using stack =  detail::container_sequential<std::stack, Args...>;

	template <typename T, typename Comparer = std::less<T>, typename ...Args>
	class vector_ordered: public vector<T, Args...>
	{
			using base = vector<T, Args...>;
		protected:
			Comparer	_comparer;
		public:
			using type				=	typename base::type;
			using value_type		=	typename type::value_type;
			using iterator			=	typename type::iterator;
			using const_iterator	=	typename type::const_iterator;

			void	insert(const value_type &value)
			{
				if (this->empty())
				{
					this->_container.push_back(value);
					return ;
				}
				iterator	it = std::lower_bound(this->begin(), this->end(), value, _comparer);
				this->_container.insert(it, value);
			}

			void	erase(const value_type &value)
			{
				iterator	it = std::lower_bound(this->begin(), this->end(), value, _comparer);
				if (it != this->end() && *it == value)
					this->_container.erase(it);
			}

			const value_type	*find(const value_type &value) const
			{
				const_iterator	it = std::lower_bound(this->begin(), this->end(), value, _comparer);
				if (it == this->end() || *it != value) return nullptr;
				return &(*it);
			}

			value_type	*find(const value_type &value)
			{
				iterator	it = std::lower_bound(this->begin(), this->end(), value, _comparer);
				if (it == this->end() || *it != value) return nullptr;
				return &(*it);
			}

			bool		exists(const value_type &value) const
			{
				return std::binary_search(this->begin(), this->end(), value, _comparer);
			}
	};

	template <typename T, typename Comparer = std::less<T>, typename ...Args>
	class vector_unique: public vector_ordered<T, Comparer, Args...>
	{
			using base = vector_ordered<T, Comparer, Args...>;
		public:
			using type				=	typename base::type;
			using value_type		=	typename type::value_type;
			using iterator			=	typename type::iterator;
			using const_iterator	=	typename type::const_iterator;

			void	insert(const value_type &value)
			{
				if (!std::binary_search(this->begin(), this->end(), value, this->_comparer))
					base::insert(value);
			}
	};
}

#endif
