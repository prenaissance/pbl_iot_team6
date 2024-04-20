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

import LogIn from './LogIn';
import Users from './Users';
import NewUser from './NewUser';

const Stack = createNativeStackNavigator();

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
      <Stack.Navigator
        screenOptions={{headerShown: false}}
      >
        {/* <Stack.Screen name="Login" component={LogIn}/> */}
        <Stack.Screen name="Users" component={Users}/>
        <Stack.Screen name="NewUser" component={NewUser}/>
      </Stack.Navigator>
    </NavigationContainer>
  )
}
export default App