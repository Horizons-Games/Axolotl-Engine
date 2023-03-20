#pragma once

#include <memory>
#include <functional>

#include <set>
#include <vector>
#include <map>
#include <list>

template<typename Pointer, typename MapKey = unsigned long long /*key of the Resources map*/>
class CollectionAwareDeleter
{
public:
	CollectionAwareDeleter() = default;
	~CollectionAwareDeleter() = default;

	void AddCollection(std::vector<Pointer*>& vector);
	void AddCollection(std::map<MapKey, Pointer*>& map);
	void AddCollection(std::set<Pointer*> set);
	void AddCollection(std::list<Pointer*> list);

	void AddCollection(std::vector<std::weak_ptr<Pointer>> vector);
	void AddCollection(std::map<MapKey, std::weak_ptr<Pointer>>& map);
	void AddCollection(std::set<std::weak_ptr<Pointer>> set);
	void AddCollection(std::list<std::weak_ptr<Pointer>> list);

	void operator() (Pointer* p)
	{
		for (const std::reference_wrapper<std::vector<Pointer*>>& vector : rawVectors)
		{
			vector.get().erase(std::remove(std::begin(vector.get()), std::end(vector.get()), p));
		}
		for (const std::reference_wrapper<std::map<MapKey, Pointer*>>& map : rawMaps)
		{
			for (auto it = std::begin(map.get()); it != std::end(map.get()); )
			{
				if (it->second == p)
				{
					it = map.get().erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		for (const std::reference_wrapper<std::set<Pointer*>>& set : rawSets)
		{
			set.get().erase(p);
		}
		for (const std::reference_wrapper<std::list<Pointer*>>& list : rawLists)
		{
			list.get().erase(std::remove(std::begin(list.get()), std::end(list.get()), p));
		}
		for (const std::reference_wrapper<std::vector<std::weak_ptr<Pointer>>>& vector : weakVectors)
		{
			for (auto it = std::begin(vector.get()); it != std::end(vector.get()); )
			{
				if (it->expired())
				{
					it = vector.get().erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		for (const std::reference_wrapper<std::map<MapKey, std::weak_ptr<Pointer>>>& map : weakMaps)
		{
			for (auto it = std::begin(map.get()); it != std::end(map.get()); )
			{
				if (it->second.expired())
				{
					it = map.get().erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		for (const std::reference_wrapper<std::set<std::weak_ptr<Pointer>>>& set : weakSets)
		{
			for (auto it = std::begin(set.get()); it != std::end(set.get()); )
			{
				if (it->expired())
				{
					it = set.get().erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		for (const std::reference_wrapper<std::list<std::weak_ptr<Pointer>>>& list : weakLists)
		{
			for (auto it = std::begin(list.get()); it != std::end(list.get()); )
			{
				if (it->expired())
				{
					it = list.get().erase(it);
				}
				else
				{
					++it;
				}
			}
		}
		delete p;
	}

private:
	std::vector<std::reference_wrapper<std::vector<Pointer*>>> rawVectors;
	std::vector<std::reference_wrapper<std::map<MapKey, Pointer*>>> rawMaps;
	std::vector<std::reference_wrapper<std::set<Pointer*>>> rawSets;
	std::vector<std::reference_wrapper<std::list<Pointer*>>> rawLists;

	std::vector<std::reference_wrapper<std::vector<std::weak_ptr<Pointer>>>> weakVectors;
	std::vector<std::reference_wrapper<std::map<MapKey, std::weak_ptr<Pointer>>>> weakMaps;
	std::vector<std::reference_wrapper<std::set<std::weak_ptr<Pointer>>>> weakSets;
	std::vector<std::reference_wrapper<std::list<std::weak_ptr<Pointer>>>> weakLists;
};

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::vector<Pointer*>& vector)
{
	rawVectors.push_back(vector);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::map<MapKey, Pointer*>& map)
{
	rawMaps.push_back(map);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::set<Pointer*> set)
{
	rawSets.push_back(set);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::list<Pointer*> list)
{
	rawLists.push_back(list);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::vector<std::weak_ptr<Pointer>> vector)
{
	weakVectors.push_back(vector);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::map<MapKey, std::weak_ptr<Pointer>>& map)
{
	weakMaps.push_back(map);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::set<std::weak_ptr<Pointer>> set)
{
	weakSets.push_back(set);
}

template<typename Pointer, typename MapKey>
inline void CollectionAwareDeleter<Pointer, MapKey>::AddCollection(std::list<std::weak_ptr<Pointer>> list)
{
	weakLists.push_back(list);
}
