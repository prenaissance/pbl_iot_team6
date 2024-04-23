import React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import SignUp from '../SignUp';
import LogIn from '../LogIn';
const Stack = createNativeStackNavigator();

function StartUpStack(){

    return(
        <Stack.Navigator
            screenOptions={{headerShown: false}}
        >
            {/* <Stack.Screen name="GetStarted" component={GetStarted} /> */}
            <Stack.Screen name="SignUp" component={SignUp} />
            <Stack.Screen name="Login" component={LogIn} />

        </Stack.Navigator>
    )
}

export default StartUpStack;