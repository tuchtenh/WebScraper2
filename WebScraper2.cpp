// WebScraper1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include "cpr/cpr.h"
#include "gumbo.h"

std::ofstream writeCsv("links.csv");

std::string extractHTML() {
    cpr::Url url = cpr::Url{ "http://www.ferry-site.dk/ferrycompanylist.php?lang=en" };
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

void search_links(GumboNode* node) {
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
        search_links(static_cast<GumboNode*>(children->data[i]));
    }
}

void search_active_links(GumboNode* node) {
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
                        search_links(static_cast<GumboNode*>(ul_children->data[i]));
                        
                    }
                }
            }
        }
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++) {
        search_active_links(static_cast<GumboNode*>(children->data[i]));
    }

}


int main()
{
    std::string page_content = extractHTML();
    GumboOutput* parsed_response = gumbo_parse(page_content.c_str());

    writeCsv << "company,link" << "\n";

    //search_title(parsed_response->root);
    //search_links(parsed_response->root);
    search_active_links(parsed_response->root);
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
