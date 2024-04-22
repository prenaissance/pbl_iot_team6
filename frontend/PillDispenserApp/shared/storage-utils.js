import AsyncStorage from '@react-native-async-storage/async-storage';

export const storeData = async(key, data) => {
    await AsyncStorage.setItem(key, data);
}

export const getData = async(key) =>{
    return await AsyncStorage.getItem(key);
}

export const getToken = async() => {
    token = await getData('token');
    return token;
  }
  