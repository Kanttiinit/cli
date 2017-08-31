#! /usr/bin/env node
const fs = require('fs');
const os = require('os');
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

const settings = {
  path: `${os.homedir()}/.kanttiinit`,
  write(data) {
    fs.writeFileSync(settings.path, JSON.stringify(data));
  },
  read() {
    try {
      return JSON.parse(fs.readFileSync(settings.path));
    } catch (e) {
      return {};
    }
  }
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
  const [opens, closes] = hours.split(' - ');
  const now = moment();
  if (now.isAfter(moment(opens, 'HH:mm')) && now.isBefore(moment(closes, 'HH:mm'))) {
    return moment(closes, 'HH:mm').fromNow();
  }
  return false;
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
  const lang = settings.read().lang || 'fi';
  const now = moment();
  const day = options.day || now;
  const isToday = now.isSame(day, 'day');

  log('Fetching restaurants...');
  const restaurants = await get(`restaurants?${restaurantsQuery}&lang=${lang}`);
  const filteredRestaurants = restaurants.slice(0, options.number);
  log(`Got ${filteredRestaurants.length} restaurants...`);
  log('Fetching menus...');
  const restaurantIds = restaurants.map(r => r.id);
  const menus = await get(`menus?restaurants=${restaurantIds.join(',')}&days=${day.format('YYYY-MM-DD')}&lang=${lang}`);
  
  let output = `\n${day.format('dddd Do [of] MMMM YYYY')}\n\n`;
  for (const restaurant of filteredRestaurants) {
    const openingHours = restaurant.openingHours[day.format('E') - 1];
    const opened = isOpen(openingHours);
    if ((!openingHours || !opened) && options.hideClosed) {
      continue;
    }
    output += `${chalk.bold(restaurant.name)} `;
    if (openingHours) {
     output += `${opened ? chalk.green(openingHours) : chalk.dim(openingHours)}`;
     if (opened && isToday) {
       output += chalk.dim(` closes ${opened}`);
     }
    } else {
      output += `closed`;
    }
    output += '\n';

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
      const query = new RegExp(options.filter, 'i');
      const filteredCourses = options.filter
        ? courses.filter(course => course.title.match(query))
        : courses;
      for (const course of filteredCourses) {
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
  .option('-f, --filter <keyword>', 'filter courses by keyword')
  .option('-n, --number <number>', 'show only n restaurants');

  program
  .command('*')
  .description('Search restaurants by restaurant or area name.')
  .action((query, options) => showMenus(`query=${query}`, options.parent));

  program
  .command('geo <address>')
  .description('Query restaurants by your address.')
  .action(async (address, options) => {
    log('Resolving location...');
    const {latitude, longitude} = await getLocation(address);
    showMenus(`location=${latitude},${longitude}`, options.parent);
  });

  program
  .command('set-lang <language>')
  .description('set and persist the preferred language (accepted values are fi and en)')
  .action(lang => {
    if (lang === 'fi' || lang === 'en') {
      settings.write({lang});
      log('Your choice has been saved.\n');
    } else {
      log('Only "fi" and "en" are supported.\n');
    }
  });

  program.parse(process.argv);
