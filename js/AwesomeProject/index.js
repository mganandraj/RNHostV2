/**
 * @format
 */

import {AppRegistry} from 'react-native';
import App from './App';
import {name as appName} from './app.json';

import { initReka } from '@office-iss/reka-react-native';
initReka();

AppRegistry.registerComponent(appName, () => App);
