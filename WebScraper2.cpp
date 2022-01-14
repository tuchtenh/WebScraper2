// WebScraper1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "cpr/cpr.h"
#include "gumbo.h"

std::ofstream writeCsv("links.csv");

std::string extractHTML() {
	//cpr::Url url = cpr::Url{ "http://www.ferry-site.dk/ferrycompanylist.php?lang=en" };
	//cpr::Url url = cpr::Url{ "http://www.ferry-site.dk/ferrycompany.php?Rid=214&lang=en" };
	cpr::Url url = cpr::Url{ "http://www.ferry-site.dk/ferry.php?id=9265421" };
	cpr::Response response = cpr::Get(url);
	return response.text;
}

void search_title(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT)
		return;
	if (node->v.element.tag == GUMBO_TAG_UL) {

		GumboVector* ul_children = &node->v.element.children; // UL children aka H3, li, li, li
		std::cout << ul_children->length << "\n";
		GumboNode* ul_first_child = static_cast<GumboNode*>(ul_children->data[0]); // h3 aka first child

		if (ul_first_child->v.element.tag == GUMBO_TAG_H3) {

			GumboVector* h3_children = &ul_first_child->v.element.children; // h3 children aka span
			GumboNode* h3_first_child = static_cast<GumboNode*>(h3_children->data[0]); // span aka first child

			if (h3_first_child->v.element.tag == GUMBO_TAG_SPAN) {

				GumboVector* span_children = &h3_first_child->v.element.children; // span children aka text
				GumboNode* span_first_child = static_cast<GumboNode*>(span_children->data[0]); // text aka first child

				if (span_first_child->type == GUMBO_NODE_TEXT) {
					std::cout << span_first_child->v.text.text << "\n";
				}
			}
		}
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		search_title(static_cast<GumboNode*>(children->data[i]));
	}
}

void search_company_links(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT)
		return;
	if (node->v.element.tag == GUMBO_TAG_A) {

		GumboVector* a_children = &node->v.element.children;
		GumboNode* a_first_child = static_cast<GumboNode*>(a_children->data[0]);
		GumboAttribute* href = gumbo_get_attribute(&node->v.element.attributes, "href");

		if (a_first_child->v.element.tag == GUMBO_TAG_FONT) {

			GumboVector* font_children = &a_first_child->v.element.children;
			GumboNode* font_first_child = static_cast<GumboNode*>(font_children->data[0]);

			if (href && font_first_child) {
				//std::cout << href->value << "\n";
				std::string link = href->value;
				if (link.rfind("ferrycompany.php?Rid=") == 0)
					writeCsv << font_first_child->v.text.text << "," << link << "\n";
			}
		}

	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		search_company_links(static_cast<GumboNode*>(children->data[i]));
	}
}

void search_active_companies(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	if (node->v.element.tag == GUMBO_TAG_UL) {

		GumboVector* ul_children = &node->v.element.children; // UL children aka H3, li, li, li
		GumboNode* ul_first_child = static_cast<GumboNode*>(ul_children->data[0]); // h3 aka first child

		if (ul_first_child->v.element.tag == GUMBO_TAG_H3) {

			GumboVector* h3_children = &ul_first_child->v.element.children; // h3 children aka span
			GumboNode* h3_first_child = static_cast<GumboNode*>(h3_children->data[0]); // span aka first child

			if (h3_first_child->v.element.tag == GUMBO_TAG_SPAN) {

				GumboVector* span_children = &h3_first_child->v.element.children; // span children aka text
				GumboNode* span_first_child = static_cast<GumboNode*>(span_children->data[0]); // text aka first child
				std::string active = span_first_child->v.text.text;

				if (span_first_child->type == GUMBO_NODE_TEXT && active.compare("Active shipping companies:") == 0) {
					std::cout << ul_children->length << "\n";
					std::cout << active << "\n";

					for (unsigned int i = 1; i < ul_children->length; i++) {
						search_company_links(static_cast<GumboNode*>(ul_children->data[i]));

					}
				}
			}
		}
	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		search_active_companies(static_cast<GumboNode*>(children->data[i]));
	}

}

void search_ship_links(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT)
		return;
	if (node->v.element.tag == GUMBO_TAG_UL) {

		GumboVector* ul_children = &node->v.element.children; // UL children aka li, li, li
		std::cout << ul_children->length << "\n";
		for (unsigned int i = 0; i < ul_children->length; i++) {
			GumboNode* ul_ith_child = static_cast<GumboNode*>(ul_children->data[i]); // ul ith child aka li

			if (ul_ith_child->v.element.tag == GUMBO_TAG_LI) {

				GumboVector* li_children = &ul_ith_child->v.element.children; // li children aka font, a

				for (unsigned int y = 0; y < li_children->length; y++) {
					GumboNode* li_ith_child = static_cast<GumboNode*>(li_children->data[y]); // li ith child aka font or a

					if (li_ith_child->v.element.tag == GUMBO_TAG_A) {

						GumboVector* a_children = &li_ith_child->v.element.children; // a children aka font
						GumboNode* a_first_child = static_cast<GumboNode*>(a_children->data[0]); // font aka first a child

						if (a_first_child->v.element.tag == GUMBO_TAG_FONT) {

							GumboVector* font_children = &a_first_child->v.element.children; // font children aka text
							GumboNode* font_first_child = static_cast<GumboNode*>(font_children->data[0]); // text aka ship name

							GumboAttribute* href = gumbo_get_attribute(&li_ith_child->v.element.attributes, "href");
							if (href) {
								std::string link = href->value;
								if (link.rfind("ferry.php?id=") == 0)
									writeCsv << font_first_child->v.text.text << "," << "http://www.ferry-site.dk/" << link << "\n";
							}
						}
					}
				}
			}
		}

	}

	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		search_ship_links(static_cast<GumboNode*>(children->data[i]));
	}
}

void search_active_ships(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT)
		return;
	if (node->v.element.tag == GUMBO_TAG_DIV) {
		GumboVector* div_children = &node->v.element.children; // div children aka center, br, br, h3, ul
		//std::cout << div_children->length << "\n";
		GumboNode* div_first_child = static_cast<GumboNode*>(div_children->data[0]); // div first child aka center
		GumboAttribute* divclass = gumbo_get_attribute(&node->v.element.attributes, "class");
		if (divclass) {
			std::string classname = divclass->value;
			//std::cout << classname << "\n";
			if (classname.compare("content") == 0) {
				std::cout << "testas" << "\n";
				for (unsigned int i = 1; i < div_children->length; i++) {
					GumboNode* div_ith_child = static_cast<GumboNode*>(div_children->data[i]); // go through div children untill h3 is found
					if (div_ith_child->v.element.tag == GUMBO_TAG_H3) {
						std::cout << "testas2" << "\n";
						GumboVector* h3_children = &div_ith_child->v.element.children; // h3 children aka text
						GumboNode* h3_first_child = static_cast<GumboNode*>(h3_children->data[0]); // text aka "active ships"
						if (h3_first_child->type == GUMBO_NODE_TEXT && std::string(h3_first_child->v.text.text).compare("Active ships:") == 0) {
							std::cout << h3_first_child->v.text.text << "\n";
							search_ship_links(static_cast<GumboNode*>(div_children->data[i + 1]));

						}
					}
				}
			}
		}
	}
	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		search_active_ships(static_cast<GumboNode*>(children->data[i]));
	}
}

void search_ship_info(GumboNode* node) {
	if (node->type != GUMBO_NODE_ELEMENT)
		return;
	// Search ship name
	if (node->v.element.tag == GUMBO_TAG_H2) {

		GumboVector* h2_children = &node->v.element.children; // h2 children aka text
		GumboNode* h2_first_child = static_cast<GumboNode*>(h2_children->data[0]); // text aka ship name

		if (h2_first_child->type == GUMBO_NODE_TEXT) {
			writeCsv << h2_first_child->v.text.text;
		}
	}

	// Search table for relevant information
	if (node->v.element.tag == GUMBO_TAG_TBODY) {

		GumboVector* tbody_children = &node->v.element.children; // tbody children aka tr,tr,tr,tr,tr,tr,tr...
		std::cout << tbody_children->length << "\n";
		for (unsigned int i = 0; i < tbody_children->length; i++) {
			GumboNode* tbody_ith_child = static_cast<GumboNode*>(tbody_children->data[i]); // tr aka ith child of tbody node

			if (tbody_ith_child->v.element.tag == GUMBO_TAG_TR) {

				GumboVector* tr_children = &tbody_ith_child->v.element.children; // tr children aka td,td (left and right column of the table)
				GumboNode* tr_first_child = static_cast<GumboNode*>(tr_children->data[0]); // left column
				GumboNode* tr_second_child = static_cast<GumboNode*>(tr_children->data[1]); // right column

				if (tr_first_child->v.element.tag == GUMBO_TAG_TD) {

					GumboVector* td_children = &tr_first_child->v.element.children; // left column children aka text
					GumboNode* td_first_child = static_cast<GumboNode*>(td_children->data[0]); // text child aka left column value
					std::string columnName = td_first_child->v.text.text;

					if (td_first_child->type == GUMBO_NODE_TEXT &&
						(columnName.compare("Building year") == 0)
						|| (columnName.compare("Building yard") == 0)
						|| (columnName.compare("Owner") == 0)
						|| (columnName.compare("Length") == 0)
						|| (columnName.compare("Breadth") == 0)
						|| (columnName.compare("Draft") == 0)
						|| (columnName.compare("GT") == 0)
						|| (columnName.compare("Machinery") == 0)
						|| (columnName.compare("Flag") == 0)) {

						if (tr_second_child->v.element.tag == GUMBO_TAG_TD) {

							GumboVector* td_children = &tr_second_child->v.element.children; // right column children aka text
							GumboNode* td_first_child = static_cast<GumboNode*>(td_children->data[0]); // text aka right column value

							if (td_first_child->type == GUMBO_NODE_TEXT) {
								writeCsv << "," << "\"" << td_first_child->v.text.text << "\"";
							}
							
						}

					}
					if (td_first_child->type == GUMBO_NODE_TEXT && (columnName.compare("Operator") == 0)) {

						if (tr_second_child->v.element.tag == GUMBO_TAG_TD) {

							GumboVector* td_children = &tr_second_child->v.element.children;  // right column children aka a
							GumboNode* td_first_child = static_cast<GumboNode*>(td_children->data[0]);  // a aka hyperlink

							if (td_first_child->v.element.tag == GUMBO_TAG_A) {

								GumboVector* a_children = &td_first_child->v.element.children; // a children aka text
								GumboNode* a_first_child = static_cast<GumboNode*>(a_children->data[0]); // text aka hyperlink display value

								if (a_first_child->type == GUMBO_NODE_TEXT) {
									writeCsv << "," << "\"" << a_first_child->v.text.text << "\"";
								}

							}

						}
					}
					if (td_first_child->type == GUMBO_NODE_TEXT && (columnName.compare("Notes") == 0)) {

						if (tr_second_child->v.element.tag == GUMBO_TAG_TD) {

							GumboVector* td_children = &tr_second_child->v.element.children; // left column children aka text
							GumboNode* td_first_child = static_cast<GumboNode*>(td_children->data[0]); // text aka right column value

							if (td_first_child->type == GUMBO_NODE_TEXT) {
								writeCsv << "," << "\"" << td_first_child->v.text.text << "\"" << "\n";
							}
							
						}
					}

				}
			}
		}

	}


	GumboVector* children = &node->v.element.children;
	for (unsigned int i = 0; i < children->length; i++) {
		search_ship_info(static_cast<GumboNode*>(children->data[i]));
	}
}


int main()
{
	std::string page_content = extractHTML();
	GumboOutput* parsed_response = gumbo_parse(page_content.c_str());

	writeCsv << "ship,Building year,Building yard,Owner,Operator,Length,Breadth,Draft,GT,Mashinery,Flag,Notes" << "\n";

	//search_title(parsed_response->root);
	//search_links(parsed_response->root);
	//search_active_companies(parsed_response->root);
	//search_active_ships(parsed_response->root);
	//std::vector<std::string> information;
	search_ship_info(parsed_response->root);
	writeCsv.close();
	gumbo_destroy_output(&kGumboDefaultOptions, parsed_response);

}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
