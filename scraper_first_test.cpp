#include <iostream>
#include "cpr/cpr.h"
#include "libxml/HTMLparser.h"
#include "libxml/xpath.h"
#include <fstream>

int main()
{
    // define the user agent for the GET request
    cpr::Header headers = {{"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/113.0.0.0 Safari/537.36"}};

    // make an HTTP GET request to retrieve the target page
    cpr::Response response = cpr::Get(cpr::Url{"https://cryptorank.io/price/bitcoin"}, headers);

    // parse the HTML document returned by the server
    htmlDocPtr doc = htmlReadMemory(response.text.c_str(), response.text.length(), nullptr, nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);

    // set the libxml2 context to the current document
    xmlXPathContextPtr context = xmlXPathNewContext(doc);

    // select the desired HTML element with an XPath selector
    xmlXPathObjectPtr element_html = xmlXPathEvalExpression((xmlChar *)"//p[contains(@class, 'sc-151230bb-0 sc-3d13e9e9-5 fNnkcW')]", context);
    
    if (element_html->nodesetval && element_html->nodesetval->nodeNr > 0)
    {
        // open the CSV file outside the loop
        std::ofstream csv_file("output.csv");
        csv_file << "element" << std::endl;

        // iterate over the elements
        for (int i = 0; i < element_html->nodesetval->nodeNr; ++i)
        {
            // get the content of the selected HTML element
            xmlNodePtr element_node = element_html->nodesetval->nodeTab[i];
            std::string element_content = reinterpret_cast<char *>(xmlNodeGetContent(element_node));

            // write the content to the CSV file
            csv_file << element_content << std::endl;
        }

        // free up the resource allocated by libxml2
        xmlXPathFreeObject(element_html);
        xmlXPathFreeContext(context);
        xmlFreeDoc(doc);

        // close the CSV file outside the loop
        csv_file.close();
        std::cout << "Data Scraped to output.csv" << '\n';
    }
    else
    {
        std::cout << "Element not found." << std::endl;
    }

    return 0;
}
