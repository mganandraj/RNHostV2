/**
 * Metro configuration for React Native
 * https://github.com/facebook/react-native
 *
 * @format
 */

const { makeMetroConfig } = require('@rnx-kit/metro-config');

const {
  exclusionList,
  resolveUniqueModule,
} = require("@rnx-kit/metro-config")

const [reactIsPath, reactIsExcludePattern] = resolveUniqueModule('react-is')
const additionalExclusions = [reactIsExcludePattern]

module.exports = {
  resolver: { 
    blockList: [exclusionList(additionalExclusions)],
    blacklistRE: [exclusionList(additionalExclusions)],
    extraNodeModules: {
      "react-is": reactIsPath,
    }
  },
  transformer: {
    getTransformOptions: async () => ({
      transform: {
        experimentalImportSupport: false,
        inlineRequires: true,
      },
    }),
  },
};