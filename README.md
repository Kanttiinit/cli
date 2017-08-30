# Kanttiinit CLI

## Install
`npm i -g kanttiinit`

## Usage
```
$ kanttiinit --help

  Usage: kanttiinit [options] [command]


  Options:

    -V, --version           output the version number
    -d, --day <day>         specify day
    -a, --address           show restaurant address
    -u, --url               show restaurant URL
    -h, --hide-closed       hide closed restaurants
    -f, --filter <keyword>  filter courses by keyword
    -h, --help              output usage information


  Commands:

    *                    Search restaurants by restaurant or area name.
    geo <address>        Query restaurants by your address.
    set-lang <language>  set and persist the preferred language (accepted values are fi and en)
```

## Examples
* Get all restaurants in an area: `kanttiinit otaniemi`
* Get all restaurants whose title matches the query: `kanttiinit aalto`
* Get nearest restaurants to an address: `kanttiinit geo otakaari 1`
* Hide closed restaurants: `kanttiinit -h keskusta`
* Show tomorrows menus: `kanttiinit -d 1 töölö`
* Only show courses which contain the word pizza: `kanttiinit -f pizza arabia`