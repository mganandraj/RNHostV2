/**
 * @format
 */

import {AppRegistry} from 'react-native';
import App from './App';
import {name as appName} from './app.json';

import { initReka } from '@office-iss/reka-react-native';
initReka();

import {NativeServices} from "./reka/AwesomeRekaDefs.g"
NativeServices.NativeService.tryCall("hello", 9999);

AppRegistry.registerComponent(appName, () => App);
