#! /usr/bin/env node

const readline = require('readline');
const chalk = require('chalk');
const fetch = require('node-fetch');
const program = require('commander');
const moment = require('moment');
const pkg = require('./package.json')

const get = async resource => {
  const response = await fetch(`https://kitchen.kanttiinit.fi/${resource}`);
  return response.json();
};

const log = message => {
  readline.clearLine(process.stdout, 0);
  readline.cursorTo(process.stdout, 0);
  process.stdout.write(message);
};

const isOpen = hours => {
  if (!hours) {
    return false;
  }
  let [open, close] = hours.split(' - ');
  open = Number(open.replace(':', ''));
  close = Number(close.replace(':', ''));
  const now = moment().format('HHmm');
  return now >= open && now < close;
};

const getLocation = async address => {
  const response = await fetch(`http://maps.googleapis.com/maps/api/geocode/json?sensor=false&address=${address}`);
  const {results} = await response.json();
  if (results.length) {
    return {
      latitude: results[0].geometry.location.lat,
      longitude: results[0].geometry.location.lng
    };
  }
  return {};
};

const showMenus = async (restaurantsQuery, options) => {
  const lang = options.lang || 'fi';
  const day = options.day || moment();

  log('Fetching restaurants...');
  const restaurants = await get(`restaurants?${restaurantsQuery}&lang=${lang}`);
  log('Fetching menus...');
  const restaurantIds = restaurants.map(r => r.id);
  const menus = await get(`menus?restaurants=${restaurantIds.join(',')}&days=${day.format('YYYY-MM-DD')}&lang=${lang}`);
  
  let output = `\n${day.format('dddd Do [of] MMMM YYYY')}\n\n`;
  for (const restaurant of restaurants) {
    const openingHours = restaurant.openingHours[day.format('E') - 1];
    const opened = isOpen(openingHours);
    if ((!openingHours || !opened) && options.hideClosed) {
      continue;
    }
    output += `${chalk.bold(restaurant.name)} `;
    if (openingHours) {
     output += `${opened ? chalk.green(openingHours) : chalk.dim(openingHours)}\n`;
    } else {
      output += `closed\n`;
    }
    if (restaurant.distance) {
      output += chalk.dim(`${restaurant.distance} meters away\n`);
    }
    if (options.address) {
      output += `${chalk.dim(restaurant.address)}\n`;
    }
    if (options.url) {
      output += `${chalk.dim(restaurant.url)}\n`;
    }
    const courses = (menus[restaurant.id] || {})[day.format('YYYY-MM-DD')];
    if (courses) {
      for (const course of courses) {
        output += `◦ ${course.title} ${chalk.dim(course.properties.join(', '))}\n`;
      }
    } else {
      output += 'No menu.\n';
    }
    output += '\n';
  }
  log(output);
};

  program
  .version(pkg.version)
  .option('-d, --day <day>', 'specify day', day => {
    if (!day) {
      return moment();
    }
    if (!isNaN(day)) {
      return moment().add({days: Number(day)});
    }
    return moment(day);
  })
  .option('-a, --address', 'show restaurant address')
  .option('-u, --url', 'show restaurant URL')
  .option('-h, --hide-closed', 'hide closed restaurants')
  .option('--set-lang', 'set the language (will be persisted)')

  program
  .command('geo [address]')
  .description('Query restaurants by your address.')
  .action(async (address, options) => {
    log('Resolving location...');
    const {latitude, longitude} = await getLocation(address);
    showMenus(`location=${latitude},${longitude}`, options.parent);
  })

  program
  .command('*')
  .description('Search restaurants by restaurant or area name.')
  .action((query, options) => showMenus(`query=${query}`, options.parent));

  program.parse(process.argv);