#ifndef FORMAT_STRING_H
#define FORMAT_STRING_H

#include <sstream>
#include <exception>
#include <memory>
#include <iomanip>
#include <iostream>


// forvard declaration
// Here should be description
template <typename... Args>
std::string make_string (const std::string& str, Args&&... args);

namespace make_string_detail {

struct value_format {
	size_t width;		// number
	size_t precision;	// .number
	char specifier;		// doxXeEfF
	char fill;			// any char
	char align;			// <>=
	char flag;			// +-#0
	
	value_format () 
		: width(0), precision(6), specifier('\0'), fill('\0'), align('\0'), flag('\0')
	{}
	
	void parse(std::istream& in);
	void set_to(std::ostream& os) const;
};

// format helper functions	
char read_last_char (const std::string& chars, std::string& format_str) 
{
	char out = '\0';
	if (chars.find(format_str.back()) != std::string::npos) {
		out = format_str.back();
		format_str.pop_back();
	}
	return out;
}

size_t string_to_int (const std::string& str, size_t default_value = 0)
{
	std::istringstream ist(str);
	size_t value = default_value;
	ist >> value;
	return value;
}

// implementation of value_format parser
void value_format::parse(std::istream& in)
{
	if (in.peek() != ':') return;
	in.ignore(1);
	std::string format_str;
	
	// read all foramt string
	while (in.peek() != '}') {
		format_str += in.peek();
		in.ignore(1);
	}

	if (format_str.empty()) {
		throw std::logic_error("Empty format string after ':'");
	}
	// read specifier
	specifier = read_last_char("doxXeEfF", format_str);
	
	// read precision and width
	if (format_str.back() == '.') {
		throw std::logic_error("Precision value expected after '.'");
	}
	
	std::string width_str;
	std::string precision_str;
	std::string buf_str;
	while (std::isdigit(format_str.back()) || format_str.back() == '.') {
		buf_str = format_str.back() + buf_str;
		format_str.pop_back();
	}
	
	std::string::size_type dot_pos = buf_str.find('.');
	
	if (dot_pos != std::string::npos) {
		width_str = buf_str.substr(0, dot_pos);
		precision_str = buf_str.substr(dot_pos + 1, buf_str.size() - dot_pos - 1);
	} else {
		width_str = buf_str; 
	}
	
	// set zeros as fill chars
	if (width_str.front() == '0' && width_str.size() > 1) {
		format_str += '0';
		width_str.erase(0, 1);
	}

	width = string_to_int(width_str, width);	
	precision = string_to_int(precision_str, precision);
	
	// read sign
	flag = read_last_char("+-#0", format_str);
	
	// read align
	align = read_last_char("<>=", format_str);
	
	// get fill char
	if (format_str.size() == 1) {
		fill = format_str.back();
		format_str.pop_back();
	}

    // fill require align 
    if (fill != '\0' && align == '\0') {
        throw std::logic_error("Expected align specification");
    }

	if (!format_str.empty()) {
		throw std::logic_error("Wrong field format");
	}
}

void value_format::set_to(std::ostream& os) const
{
	switch (specifier) {
        case '\0':
		case 'd': os << std::dec; break;
		case 'o': os << std::oct; break;
		case 'x': os << std::hex; break;
		case 'X': os << std::hex << std::uppercase; break;
		case 'e': os << std::scientific; break;
		case 'E': os << std::scientific << std::uppercase; break;
		case 'f': os << std::fixed; break;
		case 'F': os << std::fixed << std::uppercase; break;
		default: throw std::logic_error (std::string("Unknown specifier '") + specifier + "'");
	}
	
	os.precision(precision);
	os.width(width);
	
	switch (flag) {
        case '\0':
		case '-' : os << std::noshowpos; break;
		case '+' : os << std::showpos; break;
		case '#' : os << std::showbase << std::showpoint; break;
		case '0' : os << std::setfill('0'); break;
		default: throw std::logic_error (std::string("Unknown flag '") + flag + "'");
	}

	switch (align) {
        case '\0': if (flag == '0') { os << std::internal; break; }
		case '<': os << std::left; break;
		case '>': os << std::right; break;
		case '=': os << std::internal; break;
		default: throw std::logic_error (std::string("Unknown align flag '") + align + "'");
	}
	
	if (flag != '0') {
		os << std::setfill(fill == '\0' ? ' ' : fill);
	}
}

// value wrapper
struct value_base {
	virtual ~value_base(){};
	virtual void print_to(std::ostream& os) const = 0;
};

template <typename T>
struct value_holder : public value_base {
	value_holder(const T& v) : value(v) {}
	void print_to(std::ostream& os) const override { os << value; }
	const T & value;
};

template <typename T>
inline std::unique_ptr<value_base> make_value_holder_ptr(const T& t)
{
	return std::unique_ptr<value_base>(new value_holder<T>(t));
}

enum class indexing_mode { none, automatic, manual };

std::string make_string_impl (const std::string& str, const std::unique_ptr<value_base> * const values , const size_t values_count)
{
	indexing_mode mode;
	size_t next_index = 0;

	std::istringstream in(str);
	std::ostringstream out;
	char c;
	while (in.get(c))
	{
		if (c == '{') {
			if (in.peek() == '{') { 
				out << '{'; 
				in.ignore(1); 
				continue; 
			}

			size_t index;
			char next_char = in.peek();
			if (next_char == '}' || next_char == ':') {
				if (mode == indexing_mode::manual) { 
					throw std::logic_error("Cannot switch from manual field specification mode to automatic field numbering"); 
				}
				mode = indexing_mode::automatic;
				index = next_index++;
			} else if (std::isdigit(in.peek())) {
				if (mode == indexing_mode::automatic) { 
					throw std::logic_error("Cannot switch from automatic field numbering to manual field specification"); 
				}
				mode = indexing_mode::manual;
				in >> index;
			} else 	{ 
				throw std::logic_error("Field number expected"); 
			}	
			
			value_format format;
			if (in.peek() == ':') {
				format.parse(in);
			}			

			if (index >= values_count) {
				std::ostringstream buf;
				buf << index;
				throw std::logic_error(std::string("Index ") + buf.str() + " out of range"); 
			}
		
			format.set_to(out);
			values[index]->print_to(out);
			
			in.get(c);
			if (c != '}') { 
				throw std::logic_error("'}' expected"); 
			}
		} else if (c == '}') {
			if (in.peek() == '}') { 
				out << '}'; 
				in.ignore(1); 
				continue; 
			}
			throw std::logic_error("Single '}' in format string"); 
		} else {
			out << c;
		}
	}
	return out.str();
}

} // namespace make_string_detail

// implementation of interface functionality
template <typename... Args>
std::string make_string (const std::string& str, Args&&... args)
{
	using namespace make_string_detail;

	std::unique_ptr<value_base> values[sizeof...(args)] = {(make_value_holder_ptr(args))...,};
	return make_string_impl(str, values, sizeof...(args));
}


#endif // FORMAT_STRING_H

