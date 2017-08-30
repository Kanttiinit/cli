# Kanttiinit CLI

## Install
`npm i -g kanttiinit`

## Usage
```
$ kanttiinit --help

  Usage: kanttiinit [options] [command]


  Options:

    -V, --version      output the version number
    -d, --day <day>    specify day
    -a, --address      show restaurant address
    -u, --url          show restaurant URL
    -h, --hide-closed  hide closed restaurants
    --set-lang         set the language (will be persisted)
    -h, --help         output usage information


  Commands:

    geo [address]  Query restaurants by your address.
    *              Search restaurants by restaurant or area name.
```

## Examples
* `kanttiinit otaniemi`
* `kanttiinit geo otakaari 1`
