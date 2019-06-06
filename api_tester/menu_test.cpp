#include <iostream>
#include <vector>
#include <string>
#include <any>
#include <typeinfo>
#include <initializer_list>
#include <functional>
#include <eh.h>
#include <Windows.h>
// this could be done with a map<std::string, std::any>
template<typename T>
class item {
public:
	item(std::string new_title, std::function<void(T)> func) : title(new_title), function(func) {}
	std::string get_title() {
		return title;
	}
	void set_title(std::string new_title) {
		title = new_title;
	}
	void execute(T arg) {
		function(arg);
	}
	void operation(std::function<void(T)> func) {
		function = func;
	}
private:
	std::string title;
	std::function<void(T)> function;
};
template<>
class item<void> {
public:
	item(std::string new_title, std::function<void()> func) : title(new_title), function(func) {}
	std::string get_title() {
		return title;
	}
	void set_title(std::string new_title) {
		title = new_title;
	}
	void execute() {
		function();
	}
	void operation(std::function<void()> func) {
		function = func;
	}
private:
	std::string title;
	std::function<void()> function;
};

struct menu {
	menu(std::string title, std::initializer_list<std::any> items) : menu_items(items), menu_title(title) {}
	std::vector<std::any> menu_items;
	std::string menu_title;

	bool operator==(menu& input) {
		return menu_title == input.menu_title && menu_items.size() == input.menu_items.size();
	}
};

template<typename T> class homogen_handler {
public:
	homogen_handler(menu m) : handled_menu(m), display_menu(m) {
		menu_trail.push_back(m);
	}

	menu* get_menu() {
		return &display_menu;
	}

	void descend(std::any selection) {
		next_menu(display_menu, selection);
	}

	void ascend() {
		if (menu_trail.size() >= 2) {
			display_menu = (menu_trail.end()[-2]);
			menu_trail.erase(menu_trail.end() - 1);
		}
	}

	std::string get_tail() {
		std::string sep, output;
		for (auto i_menu : menu_trail) {
			output += sep + i_menu.menu_title;
			sep = " -> ";
		}
		return output;
	}

private:
	menu handled_menu;
	menu display_menu;
	std::vector<menu> menu_trail;

	const std::type_info& t_value = typeid(item<T>);
	const std::type_info& t_function = typeid(item<void>);
	const std::type_info& t_menu = typeid(menu);

	void print_menu(menu sample, int level) {

		std::cout << std::string(level * 4, ' ') << sample.menu_title << std::endl;
		for (auto menu_item : sample.menu_items) {
			if (menu_item.type() == t_function) {
				std::cout << std::string(level * 4 + 4, ' ') << std::any_cast<item<void>>(menu_item).get_title() << std::endl;
			}
			else if (menu_item.type() == t_menu) {
				print_menu(std::any_cast<menu>(menu_item), level + 1);
			}
			else if (menu_item.type() == t_value) {
				std::cout << std::string(level * 4 + 4, ' ') << std::any_cast<item<T>>(menu_item).get_title() << std::endl;
			}
		}
	}

	void next_menu(menu _menu, std::any selection_item) {
		if (selection_item.type() == t_menu) {
			for (auto item : _menu.menu_items) {
				if (item.type() == t_menu && std::any_cast<menu>(item) == std::any_cast<menu>(selection_item)) {
					display_menu = std::any_cast<menu>(item);
					menu_trail.push_back(display_menu);
					return;
				}
			}
		}

	}
};
template<typename T> void print_menu(menu sample, int level) {
	const std::type_info& t_value = typeid(item<T>);
	const std::type_info& t_function = typeid(item<void>);
	const std::type_info& t_menu = typeid(menu);
	std::cout << std::string(level * 4, ' ') << sample.menu_title << std::endl;
	for (auto menu_item : sample.menu_items) {
		if (menu_item.type() == t_function) {
			std::cout << std::string(level * 4 + 4, ' ') << std::any_cast<item<void>>(menu_item).get_title() << std::endl;
		}
		else if (menu_item.type() == t_menu) {
			print_menu<int>(std::any_cast<menu>(menu_item), level + 1);
		}
		else if (menu_item.type() == t_value) {
			std::cout << std::string(level * 4 + 4, ' ') << std::any_cast<item<T>>(menu_item).get_title() << std::endl;
		}
	}
}

std::ostream& operator<<(std::ostream& os, menu const& m) {
	print_menu<int>(m, 0);
	return os;
}

int main() {
	auto sample = [](int a) {std::cout << "    " << a << std::endl; };
	auto sample1 = []() {std::cout << "    " << 1 << std::endl; };

	menu aa("Sample Title", {
			item<int>("s", sample),
			item<void>("ss", sample1),
			item<int>("s", sample),
			item<int>("ss", sample),
			item<int>("s", sample),
			item<void>("s", sample1),
			menu("Hello World", {
				menu("Hello World", {
					menu("Hello World", {
						item<int>("s", sample),
						item<int>("ss", sample)
					}),
					item<int>("ss", sample)
				}),
				item<int>("ss", sample)
			}),
			menu("Hello World", {
				menu("Hello World", {
					menu("Hello World", {
						menu("Hello World", {
							menu("Hello World", {
								menu("Hello World", {
									item<int>("s", sample),
									item<int>("ss", sample)
								}),
								item<int>("ss", sample)
							}),
							item<int>("ss", sample)
						}),
						item<int>("ss", sample)
					}),
					item<int>("ss", sample)
				}),
				item<int>("ss", sample),
				item<int>("ss", sample),
				item<int>("ss", sample),
				item<int>("ss", sample),
				item<int>("ss", sample)
			}),
		});

	homogen_handler<int> a(aa);
	a.ascend();
	menu* displayed_menu = a.get_menu();
	std::cout << a.get_tail() << std::endl << *displayed_menu << std::endl;
	a.descend((*displayed_menu).menu_items[6]);
	std::cout << a.get_tail() << std::endl << *displayed_menu << std::endl;
	a.ascend();
	std::cout << a.get_tail() << std::endl << *displayed_menu << std::endl;
}