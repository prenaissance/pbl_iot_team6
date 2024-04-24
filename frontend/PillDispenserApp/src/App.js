/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React, {useState, useEffect, useContext} from 'react';
import {
  useColorScheme,
} from 'react-native';
import { Colors } from "react-native/Libraries/NewAppScreen"
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import { getData } from '../shared/storage-utils';
import StartUpStack from './nav/StartUpStack';
import MyTabs from './nav/MyTabs';
import {useLogin } from './context/LoginProvider';
import LoginProvider from './context/LoginProvider';
import MainNavigator from './nav/MainNavigator';
function App (){
  const isDarkMode = useColorScheme() === "dark"
  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter
  }
 
  return(
    <LoginProvider>
        <NavigationContainer>

      <MainNavigator/>
      </NavigationContainer>

    </LoginProvider>

    
  )
}
export default App