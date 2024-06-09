import React, { useContext, useEffect, useState } from 'react';
import { NavigationContainer } from '@react-navigation/native';
import StartUpStack from './StartUpStack';
import MyTabs from './MyTabs';
import { useLogin } from '../context/LoginProvider';
import { View } from 'react-native';
import { getData } from '../../shared/storage-utils';
import SplashScreen from '../SplashScreen';

function MainNavigator(){
  const [isLoading, setIsLoading] = useState(false);
  useEffect(() => {
    const checkLogIn = async () => {
      setIsLoading(true);
      if (await getData('token') != null){
        setIsLoggedIn(true);
      } 
      setIsLoading(false);
    }
    
    checkLogIn();
    
  }, [])
    const {isLoggedIn, setIsLoggedIn} = useLogin();
    return(
      
          isLoading? (
            <SplashScreen/>
          ) :
          !isLoggedIn ? (
            <StartUpStack/>
          ) : (
            <MyTabs/>
          )

        
    )
}

export default MainNavigator;