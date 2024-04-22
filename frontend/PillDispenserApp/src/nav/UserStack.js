import React from 'react';
import { createNativeStackNavigator } from '@react-navigation/native-stack';
import NewUser from '../NewUser';
import Users from '../Users';

const Stack = createNativeStackNavigator();

function UserStack(){

    return(
        <Stack.Navigator initialRouteName='Users'
            screenOptions={{headerShown: false}}
        >
            <Stack.Screen name="Users" component={Users} />
            <Stack.Screen name="NewUser" component={NewUser} />
        </Stack.Navigator>
    )
}

export default UserStack;