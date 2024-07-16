import "./App.css"

// Pages
import HomePage from "./Pages/HomePage.js"
import HardwarePage from "./Pages/HardwarePage.js"
import AnimationPage from "./Pages/AnimationPage.js"
import SoftwarePage from "./Pages/SoftwarePage.js"
import CADPage from "./Pages/CadPage.js"

// Components
import Navbar from "./Components/navbar"
import Footer from "./Components/footer"
import { Switch } from './Components/switch.js'

// React hooks
import React, { useState } from "react"
import { Route, Routes } from "react-router-dom"

export default function App() {

  const logState = state => {
    console.log('Toggled:', state)
  }

  const mode = toggled => {
    switch (toggled) {
      case true:
        return 'dark';
      default:
        return 'light';
    }
  }

  // console.log(window.location) get file location of page
  return (
    <div className={mode}>
      <Navbar />
      {/* <img src={require("./Images/moon-stars-svgrepo-com.svg")}></img>
      <Switch label = "toggle me" toggled = {true} onClick={logState}/> */}
      <div className="divider">
        <Routes>
          <Route path="/" element={<HomePage />} />
          <Route path="/hardware" element={<HardwarePage />} />
          <Route path="/animation" element={<AnimationPage />} />
          <Route path="/software" element={<SoftwarePage />} />
          <Route path="/cad" element={<CADPage />} />
        </Routes>
      </div>
      <Footer />
    </div>
  );
}

// export default App; // Can also be added at end of code