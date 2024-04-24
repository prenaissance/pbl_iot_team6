import React, { useContext } from 'react';
import { NavigationContainer } from '@react-navigation/native';
import StartUpStack from './StartUpStack';
import MyTabs from './MyTabs';
import { useLogin } from '../context/LoginProvider';
import { View } from 'react-native';

function MainNavigator(){
    const {isLoggedIn} = useLogin();
    return(
      
          !isLoggedIn ? (
            <StartUpStack/>
          ) : (
            <MyTabs/>
          )

        
    )
}

export default MainNavigator;