// server.js
const express = require('express');
const bodyParser = require('body-parser');
const app = express();
const port = process.env.PORT || 3000;

app.use(bodyParser.json());

// Function to simulate real-time sensor data
function getDynamicSensorData() {
  const temperature = Math.floor(Math.random() * 10) + 20;  // 20-29 °C
  const humidity = Math.floor(Math.random() * 20) + 60;     // 60-79%
  const soilMoisture = (Math.random() * 20 + 30).toFixed(2);  // 30.00-50.00%
  return { temperature, humidity, soilMoisture };
}

app.post('/webhook', (req, res) => {
  const intentName = req.body.queryResult.intent.displayName;
  let responseText = "";
  const sensorData = getDynamicSensorData(); // Get simulated dynamic sensor data

  // Intent: GetRealTimeSensorData
  if (intentName === "GetRealTimeSensorData") {
    responseText = `Right now, the temperature is ${sensorData.temperature} °C, the humidity is ${sensorData.humidity}%, and the soil moisture is ${sensorData.soilMoisture}%.`;
  }
  // Intent: ExplainSensorData
  else if (intentName === "ExplainSensorData") {
    responseText = "Temperature shows how warm your field is, which affects plant growth. Humidity tells you how much moisture is in the air, and soil moisture indicates the water available in the soil for plants.";
  }
  // Intent: CropRecommendations
  else if (intentName === "CropRecommendations") {
    const currentTemp = sensorData.temperature;
    if (currentTemp < 23) {
      responseText = "It’s a bit cool now. Consider planting leafy greens like spinach or lettuce.";
    } else if (currentTemp >= 23 && currentTemp <= 28) {
      responseText = "The conditions are perfect. Tomatoes or peppers would be a great choice.";
    } else {
      responseText = "It’s warm now. Crops like eggplant or okra might do well.";
    }
  }
  // Fallback if intent not recognized
  else {
    responseText = "I’m sorry, I didn’t understand that. Please try asking differently.";
  }

  // Return the dynamically generated response
  return res.json({
    fulfillmentText: responseText
  });
});

app.listen(port, () => {
  console.log(`Webhook server is running on port ${port}`);
});
