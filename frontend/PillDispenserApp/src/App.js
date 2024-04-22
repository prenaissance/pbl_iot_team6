/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * @format
 */

import React from 'react';
import {
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  useColorScheme,
  View,
  Button,
} from 'react-native';
import { Colors, DebugInstructions, Header, LearnMoreLinks, ReloadInstructions } from "react-native/Libraries/NewAppScreen"
import { NavigationContainer } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';

import LogIn from './LogIn';
import Users from './Users';
import NewUser from './NewUser';
import SignUp from './SignUp';
import { getData, getToken } from '../shared/storage-utils';
import AsyncStorage from '@react-native-async-storage/async-storage';
import StartUpStack from './nav/StartUpStack';
import UserStack from './nav/UserStack';
import ContainerStack from './nav/ContainerStack';
import Notifications from './Notifications';
const Stack = createNativeStackNavigator();
const Tab = createBottomTabNavigator();
function App (){
  const isDarkMode = useColorScheme() === "dark"
  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter
  }

  
  return(
    // <View>
    //   <Button title='Led One On' />
    //   <Button title='Led Two On' />
    //   <Button title='Led Three On' />
    // </View>
    // <LogIn/>
    <NavigationContainer>
      {
        getToken == null ? (
          <StartUpStack/>
        ) : (
          <Tab.Navigator
            screenOptions={{headerShown: false}}
          >
            <Tab.Screen name="UserStack" component={UserStack}/>
            <Tab.Screen name="ContainerStack" component={ContainerStack}/>
            <Tab.Screen name="Notifications" component={Notifications}/>
          </Tab.Navigator>
        )

      }
      

      
    </NavigationContainer>
  )
}
export default App