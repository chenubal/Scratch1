#pragma once
#include <span>
#include <memory>

using Element = int;
using SpanE = std::span<Element>;

struct HandlerInterace
{
	virtual void handle(SpanE const& elements) = 0;
};

enum class Method { A, B };

std::unique_ptr<HandlerInterace> makeHandler(Method method);
