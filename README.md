# Kanttiinit CLI

## Install
To do...

Requires curl.

## Usage
```
$ kanttiinit --help
Kanttiinit.fi command-line interface.
Usage:
  Kanttiinit CLI [OPTION...]

  -q, --query arg     Search restaurants by restaurant or area name.
  -g, --geo arg       Search restaurants by location.
  -d, --day arg       Specify day. (default: 0)
  -f, --filter arg    Filter courses by keyword.
  -n, --number arg    Show only n restaurants.
  -v, --version       Display version.
  -a, --address       Show restaurant address.
  -u, --url           Show restaurant URL.
  -h, --hide-closed   Hide closed restaurants.
      --set-lang arg  Save the preferred language (fi or en).
      --help          Display help.

Get all restaurants in a specific area:
kanttiinit -q otaniemi

Get restaurants by restaurant name:
kanttiinit -q unicafe

Get restaurants close to a location:
kanttiinit -g Otakaari 8

Only list courses that match a certain keyword:
kanttiinit -q töölö -f salad

See menus for tomorrow:
kanttiinit -q alvari -d 1
```

## Examples
* Get all restaurants in an area: `kanttiinit -q otaniemi`
* Get all restaurants whose title matches the query: `kanttiinit -q aalto`
* Get nearest restaurants to an address: `kanttiinit -g otakaari 1`
* Hide closed restaurants: `kanttiinit -h -q keskusta`
* Show tomorrows menus: `kanttiinit -d 1 -q töölö`
* Only show courses which contain the word pizza: `kanttiinit -f pizza -q arabia`