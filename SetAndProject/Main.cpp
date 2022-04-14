#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <optional>
#include <functional>

#pragma once

struct SetAttibutes {};
struct Buffer {};

using SetId = std::string;

namespace jh
{
	template<class Function, class Index = unsigned long>
	struct Loop
	{
		using Result = std::invoke_result_t<Function, Index>;
		struct iterator
		{
			iterator(Function f, Index start = 0) : m_counter(start), m_function(f) {}
			Result operator*() const { return m_function(m_counter); }
			iterator& operator++() { ++m_counter; return *this; }
			iterator operator++(int) { auto tmp = *this; ++(*this); return tmp; }
			bool operator!=(iterator const& other) const { return other.m_counter != m_counter; }
			bool operator==(iterator const& other) const { return other.m_counter == m_counter; }
		protected:
			Index m_counter;
			Function m_function; // R=f(I)
		};

		Loop(Function f, Index endIndex) : m_begin(iterator(f, 0)), m_end(iterator(f, endIndex)) {}
		Loop(Index endIndex) : Loop([](auto i) { return i; }, endIndex) {}
		auto begin() { return m_begin; }
		auto end() { return m_end; }
		auto const begin() const { return m_begin; }
		auto const end() const { return m_end; }
		iterator m_begin, m_end;
	};

	// CTAD
	template <typename Index>
	Loop(Index)->Loop<std::function<Index(Index)>, Index>;
}

struct BufferAccess
{
	template<typename T>
	using RW = std::reference_wrapper<T>;

	//virtual std::optional<RW<Buffer const>>  get(size_t) const = 0;
	//virtual std::optional<RW<Buffer>> get(size_t) = 0;
	virtual Buffer const& at(size_t) const = 0;
	virtual Buffer& at(size_t) = 0;
	virtual Buffer const& operator[](size_t) const = 0;
	virtual Buffer& operator[](size_t) = 0;

	virtual Buffer& insert(Buffer const&, size_t pos) = 0;
	virtual Buffer& insert(Buffer &&, size_t pos) = 0;
	virtual std::unique_ptr<Buffer> release(size_t) = 0;
	virtual bool remove(size_t) = 0;

	virtual ~BufferAccess() = 0;
};
struct AbstractSet : public BufferAccess
{
	template<typename T>
	using RW = std::reference_wrapper<T>;

	virtual size_t size() const = 0;
	virtual size_t numSubSets() const = 0;
	virtual bool hasSet(SetId const&) const = 0;

	virtual std::unordered_set<SetId> getSubIds() const = 0;

	//virtual std::optional<RW<AbstractSet const>>  get(SetId const&) const = 0;
	//virtual std::optional<RW<AbstractSet const>> get(SetId const&) = 0;

	virtual AbstractSet& at(SetId const&) = 0;
	virtual AbstractSet& operator[](SetId const&) = 0;
	virtual AbstractSet const& at(SetId const&) const = 0;
	virtual AbstractSet const& operator[](SetId const&) const = 0;

	virtual AbstractSet& add(AbstractSet const&, SetId const&) = 0;
	virtual AbstractSet& add(AbstractSet&&, SetId const&) = 0;
	virtual std::unique_ptr<AbstractSet> release(SetId const&) = 0;
	virtual bool remove(SetId const&) = 0;

	virtual SetAttibutes const& getAttributes() const = 0;
	virtual SetAttibutes& getAttributes() = 0;

	virtual ~AbstractSet() = 0;
};

using PT = std::function<std::optional<Buffer&>(size_t)>;
//jh::Loop<PT,size_t> BufferProxy(AbstractSet& s) { return jh::Loop<PT, size_t>([&s](size_t i) { return s.getBuffer(i); s.size(); }); }
//auto SubSetProxy(AbstractSet& s) { return RTE::Loop([&s, k = s.getSubIds()](unsigned i) { return s.getSet(*(k.begin() + i)); s.size(); }); }

Buffer b;
std::optional<Buffer> fun(bool ok) { if (ok) return  { b }; else return{}; }

struct ProjectProperties : public AbstractSet
{};

struct AbstractProject : public AbstractSet
{
	virtual ProjectProperties const& getProperties() const = 0;
	virtual ProjectProperties& getProperties() = 0;

	virtual ~AbstractProject() = 0;

};



int main(int, char**)
{

	std::cout << "TODO\n";
	return 0;

}

