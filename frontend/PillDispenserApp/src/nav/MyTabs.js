import React from 'react';
import { createBottomTabNavigator } from '@react-navigation/bottom-tabs';
import UserStack from './UserStack';
import ContainerStack from './ContainerStack';
import Notifications from '../Notifications';

const Tab = createBottomTabNavigator();
function MyTabs(){
    return(
        <Tab.Navigator
            screenOptions={{headerShown: false}}
          >
            <Tab.Screen name="UserStack" component={UserStack}/>
            <Tab.Screen name="ContainerStack" component={ContainerStack}/>
            <Tab.Screen name="Notifications" component={Notifications}/>
          </Tab.Navigator>
    )
}

export default MyTabs;