import React from "react";
import { Text, View } from "react-native";
import { globalStyles } from "../shared/style-sheet";

function Notifications({navigation}){

    return(
        <View>
            <Text style={[globalStyles.title, globalStyles.header]}>
                NOTIFICATIONS
            </Text>
        </View>
    )
}

export default Notifications;