#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>

class ini_parser
{
public:
	ini_parser(const std::string& fileName)
	{
		parse(fileName);
	}

	template<typename T>
	T get_value(const std::string& key)
	{
		auto it{ map.find(key) };

		if (it == map.end())
		{
			auto dot{ key.find('.') };
			std::string section{ key.substr(0, dot) };
			std::string varName{ key.substr(dot + 1) };
			std::ostringstream list{};

			list << "variable \"" + varName + "\" not found in section [" + section + "]\n";
			list << "list of available variables:\n";

			for (auto [key, value] : map)
			{
				if (key.starts_with(section + '.'))
				{
					list << key.substr(section.size() + 1) << '\n';
				}
			}

			throw std::runtime_error(list.str());
		}

		if constexpr (std::is_same_v<T, std::string>)
		{
			return it->second;
		}
		else
		{
			std::istringstream strStream(it->second);

			T value{};

			strStream >> value;

			if (strStream.fail())
			{
				throw std::runtime_error("Error: variable \"" + key + "\" cannot be converted to requested type");
			}

			return value;
		}
	}

private:
	void parse(const std::string& fileName)
	{
		std::ifstream file(fileName);

		if (!file.is_open()) 
		{
			throw std::runtime_error("Error: file \"" + fileName + "\" not found");
		}

		int row{};
		std::string line{};
		std::string section{};

		while (std::getline(file, line))
		{
			++row;

			line = trim(line);

			if (line.empty() || line[0] == ';')
			{
				continue;
			}

			size_t index{ line.find(';') };

			if(index != std::string::npos) 
			{
				line.erase(index);
			}

			size_t begin{ line.find('[') };
			size_t end{ line.find(']') };

			if (begin != std::string::npos && end != std::string::npos && begin < end)
			{
				section = trim(line.substr(begin + 1, end - begin - 1));

				continue;
			}

			if (section.empty())
			{
				throw std::runtime_error("Error: variable outside the section, line: " + std::to_string(row));
			}

			index = line.find('=');
			std::string varName{ trim(line.substr(0, index)) };
			std::string varValue{ trim(line.substr(index + 1)) };

			if (index == std::string::npos || varName.empty() || varValue.empty())
			{
				throw std::runtime_error("Error: invalid variable, line: " + std::to_string(row));
			}

			map[section + '.' + varName] = varValue;
		}

		file.close();
	}

	std::string trim(const std::string& str)
	{
		size_t start = str.find_first_not_of(" \t\n\r");
		size_t end = str.find_last_not_of(" \t\n\r");
		return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
	}

private:
	std::unordered_map<std::string, std::string> map;
};

int main()
{
	try
	{
		ini_parser file("file - Copy.ini");

		
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what();
	}
	
	return EXIT_SUCCESS;
}