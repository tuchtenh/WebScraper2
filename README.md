# WebScraper2

Web scraper written in C++ using [curl](https://curl.se/) and [Gumbo](https://github.com/google/gumbo-parser) libraries.

Given a link of shipping companies, looks through active company ships and gathers these parameters of the ship:
* Building year
* Building yard
* Owner
* Operator
* Length
* Breadth
* Draft
* GT
* Mashinery
* Flag
* Notes

Scraped data is stored into a CSV file.

All active shipping comapany links:
* Active shipping companies.csv

All active ships of active shipping companies:
* Active ships.csv

All ship data:
* Ships.csv
