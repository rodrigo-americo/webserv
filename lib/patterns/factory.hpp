#ifndef FACTORY_HPP
# define FACTORY_HPP

# include <map>

# include "singleton.hpp"

namespace patterns
{
	template <typename Derived, typename MapKey, typename Item, typename CreateInput>
	class factory: public patterns::singleton<Derived>
	{
		// friend class patterns::singleton<factory>;
		protected:
			typedef typename std::map<MapKey, Item*>::iterator	iterator;
			std::map<MapKey, Item*>	_items;

			factory() {};
			factory(const factory &);
			factory	&operator=(const factory &);
			~factory() {};

		public:
			static void	registerItem(Item *item)
			{
				Derived	&instance = Derived::getInstance();
				instance._addItem(item);
			}
			static Item	*create(CreateInput router)
			{
				Derived	&instance = Derived::getInstance();
				return instance._create(router);
			}
	};

	template <typename Derived, typename FactoryConcrete>
	class factory_register
	{
		private:
			static bool	_register;
		public:
			factory_register() { (void)_register; }
	};

	template <typename Derived, typename FactoryConcrete>
	bool	registerFuntion()
	{
		FactoryConcrete::registerItem(new Derived());
		return true;
	}

	template <typename Derived, typename FactoryConcrete>
	bool	factory_register<Derived, FactoryConcrete>::_register = registerFuntion<Derived, FactoryConcrete>();
}

#endif
