#ifndef SOURCE_DEPLETED_H
#define SOURCE_DEPLETED_H

#include <stdexcept>
#include <string>


class source_depleted : std::runtime_error {
public:
	source_depleted(const std::string& what_arg) : runtime_error(what_arg) {}
};


#endif
