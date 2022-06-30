#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <optional>
#include <functional>
#include <map>
#include <xutility>

#pragma once

struct SetAttibutes {};
struct Buffer 
{
	Buffer(int i = 0) : my_i(i) {}
	int my_i;
};

using SetId = std::string;

namespace Lv
{
	template<typename T>
	using Ref = std::reference_wrapper<T>;
	template<typename T>
	using OR = std::optional<Ref<T>>;

	struct BufferAccess
	{
		using value_type = Buffer;

		virtual size_t size() const = 0;

		virtual value_type const& at(size_t) const = 0;
		virtual value_type& at(size_t) = 0;

		virtual value_type const& operator[](size_t) const = 0;
		virtual value_type& operator[](size_t) = 0;

   	virtual value_type& insert(value_type const&, size_t pos) = 0;
		virtual value_type& insert(value_type &&, size_t pos) = 0;

		virtual std::unique_ptr<value_type> release(size_t) = 0;
		virtual bool remove(size_t) = 0;

		virtual ~BufferAccess() = 0;
	};

	template<class BA>
	struct iteratorBA
	{
		iteratorBA(BA& client, unsigned start=0): client(client), c(start) {}
		auto& operator*() { return client.at(c); }
		auto& operator->() { return *(*this); }

		auto const& operator*() const { return client.at(c); }
		auto const& operator->() const { return *(*this); }

		auto& operator++() { ++c; return *this; }
		auto operator++(int) { return iteratorBA(client,c++); }

		bool operator==(iteratorBA const& o) const { return c == o.c; }
		bool operator!=(iteratorBA const& o) const { !(*this == o;) }
	protected:
		BA& client;
		unsigned c;
	};

	struct AbstractSet : public BufferAccess
	{
		virtual size_t numSubSets() const = 0;
		virtual bool hasSet(SetId const&) const = 0;

		virtual std::unordered_set<SetId> getSubIds() const = 0;

		virtual AbstractSet& at(SetId const&) = 0;
		virtual AbstractSet const& at(SetId const&) const = 0;

		virtual AbstractSet& operator[](SetId const&) = 0;
		virtual AbstractSet const& operator[](SetId const&) const = 0;

		virtual AbstractSet& add(AbstractSet const&, SetId const&) = 0;
		virtual AbstractSet& add(AbstractSet&&, SetId const&) = 0;
		virtual std::unique_ptr<AbstractSet> release(SetId const&) = 0;
		virtual bool remove(SetId const&) = 0;

		virtual SetAttibutes const& getAttributes() const = 0;
		virtual SetAttibutes& getAttributes() = 0;

		auto begin() { return iteratorBA<AbstractSet>(*this, 0); }
		auto end() { return iteratorBA<AbstractSet>(*this, size()); }
		auto begin() const { return iteratorBA<AbstractSet const>(*this, 0); }
		auto end() const { return iteratorBA<AbstractSet const>(*this, size()); }

		virtual ~AbstractSet() = 0;
	};
	struct ProjectProperties : public Lv::AbstractSet
	{};

	struct AbstractProject : public Lv::AbstractSet
	{
		virtual ProjectProperties const& getProperties() const = 0;
		virtual ProjectProperties& getProperties() = 0;
	};
}

struct MemSet : public Lv::AbstractSet
{
	using typename Lv::AbstractSet::value_type;
	MemSet() = default;
	//-------------------------------------------------------
	size_t size() const override { return data.size(); }
	value_type const& at(size_t i) const override { return data.at(i); }
	value_type& at(size_t i) override { return data.at(i); }
	value_type const& operator[](size_t i) const override { return data[i]; }
	value_type& operator[](size_t i) override  { return data[i]; }
	value_type& insert(value_type const& v, size_t pos) override { data.insert(ip(pos), v); return data.at(pos); }
	value_type& insert(value_type && v, size_t pos)  override {data.insert(ip( pos), std::move(v)); return data.at(pos); }
	std::unique_ptr<value_type> release(size_t pos) override { auto t = data.at(pos); remove(pos); return std::make_unique<value_type>(std::move(t)); }
	bool remove(size_t i) override { data.erase(ip(i)); }
	//-------------------------------------------------------
	//-------------------------------------------------------

	auto ip(size_t i) { return std::next(data.begin(), i); }
	std::vector<value_type> data;
	std::map<SetId, AbstractSet> subsets;
};




int main(int, char**)
{

	std::cout << "TODO\n";
	return 0;

}

