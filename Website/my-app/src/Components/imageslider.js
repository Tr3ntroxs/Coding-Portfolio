import { useState } from "react";

const ImageSlider = ({ slides }) => {
    const [currentIndex, setCurrentIndex] = useState(0);

    const container = {
        width: "500px",
        height: "280px",
        margin: "0 auto"
    };

    const slider = {
        height: "100%",
        position: "relative"
    };

    const slide = {
        width: "100%",
        height: "100%",
        borderRadius: "10px",
        backgroundPosition: "center",
        backgroundSize: "cover",
        backgroundImage: `url(${slides[currentIndex].url})`
    };

    const rightarrow = {
        position: "absolute",
        top: "50%",
        transform: "translate(0, -50%)",
        right: "12px",
        fontSize: "45px",
        color: "#000000",
        zIndex: 1,
        cursor: "pointer",
      };
      
      const leftarrow = {
        position: "absolute",
        top: "50%",
        transform: "translate(0, -50%)",
        left: "12px",
        fontSize: "45px",
        color: "#000000",
        zIndex: 1,
        cursor: "pointer",
      };

      const dots = {
        display: "flex",
        justifyContent: "center",
      };
      
      const dot = {
        margin: "0 3px",
        cursor: "pointer",
        fontSize: "20px",
      };
    
    // Arrow functions
    const goBack = () => {
        const isFirst = currentIndex === 0;
        const newIndex = isFirst ? slides.length - 1 : currentIndex - 1;
        setCurrentIndex(newIndex);
    };

    const goForth = () => {
        const isLast = currentIndex === slides.length - 1;
        const newIndex = isLast ? 0 : currentIndex + 1;
        setCurrentIndex(newIndex);
    };

    const goToSlide = slideIndex => {
        setCurrentIndex(slideIndex);
    }

    return (
        <div style={container}>
            <div style={slider}>
                <div>
                    <div style={leftarrow} onClick={goBack}>L</div>
                    <div style={rightarrow} onClick={goForth}>R</div>
                </div>
                <div style={slide}></div>
                <div style = {dots}>
                    {slides.map((slide, slideIndex) => (
                        <div key={slideIndex} style={dot} onClick={() => goToSlide(slideIndex)}>0</div>
                    ))}
                </div>
            </div>
        </div>
    )
}

export default ImageSlider;