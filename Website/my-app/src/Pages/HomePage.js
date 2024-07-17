import "./HomePage.css"
import { FaPaperPlane, FaYoutube, FaInstagram, FaLinkedinIn } from "react-icons/fa";
import { ImPhone } from "react-icons/im";

export default function HomePage() {

    return (
        <div className="page">
            <h1>Limitations are rules meant to be broken</h1>
            
            <img src={require("../Images/background.jpg")}></img>


            <div className="about">
                <div className="container"></div>
            </div>


            <div className="services">
                <div className="container"></div>
            </div>


            <div className="contact">
                <div className="container">
                    <div className="row">
                        <div className="contact-left">
                            <h1>Contact Me</h1>
                            <p><i><FaPaperPlane /></i>testingllamas@gmail.com</p>
                            <p><i><ImPhone /></i>978-223-7494</p>
                            <div className="social">
                                <a href="https://www.instagram.com/trent.maffeo/?next=%2F"><FaInstagram /></a>
                                <a href="https://www.youtube.com/channel/UCZmEW-klHYOcHQWprsOgCOw"><FaYoutube /></a>
                                <a href="https://www.linkedin.com/in/trent-maffeo-270747228/"><FaLinkedinIn /></a>
                            </div>
                            <a href="../Images/resume.pdf" download className="btn">Download CV</a>
                        </div>
                        <div className="contact-right">
                            <form className="submit-to-google-sheet"> 
                                <input type="text" name="Name" placeholder="Your Name" required></input>
                                <input type="email" name="email" placeholder="Your Email" required></input>
                                <textarea name="Message" rows="6" placeholder="Your Message"></textarea>
                                <button type="submit" className="btn">Submit</button>
                            </form>
                            {/* <span id="msg"></span> */}
                        </div>
                    </div>
                </div>
                {/* <div class="copyright">
                    <p>Copyright © Trent. Crafted with <i class="fa fa-heart fa-beat"></i> by Trent Maffeo</p>
                </div> */}
            </div>
        </div>
    )
}