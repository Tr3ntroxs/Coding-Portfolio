import "./HomePage.css"
import { AiFillAccountBook,  } from "react-icons/ai";
import { FontAwesomeIcon } from '@fortawesome/react-fontawesome'
import { faCoffee } from '@fortawesome/free-solid-svg-icons'

export default function HomePage() {
    const element = <FontAwesomeIcon icon={faCoffee} />

    return (
        <div className="page">
            <h1>Limitations are rules meant to be broken</h1>
            <AiFillAccountBook />
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
                    ReactDOM.render(element, document.body)
                        <div className="contact-left">
                            <h1>Contact Me</h1>
                            <p><i className="fa fa-paper-plane"></i>testingllamas@gmail.com</p>
                            <p><i className="fa fa-phone"></i>978-223-7494</p>
                            <div className="social">
                                <a href="https://www.instagram.com/trent.maffeo/?next=%2F"><i className="fa fa-instagram"></i></a>
                                <a href="https://www.youtube.com/channel/UCZmEW-klHYOcHQWprsOgCOw"><i className="fa fa-youtube"></i></a>
                                <a href="https://www.linkedin.com/in/trent-maffeo-270747228/"><i className="fa fa-linkedin"></i></a>
                            </div>
                            <a href="../Images/resume.pdf" download className="btn">Download CV</a>
                        </div>
                        <div className="contact-right">
                            <form className="submit-to-google-sheet"> 
                                <input type="text" name="Name" placeholder="Your Name" required></input>
                                <input type="email" name="email" placeholder="Your Email" required></input>
                                <textarea name="Message" rows="6" placeholder="Your Message"></textarea>
                                <button type="submit" className="btn">Submit</button>
                                {/* H E Y, fix up the icons not appearing */}
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