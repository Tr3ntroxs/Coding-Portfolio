import { Link, useMatch, useResolvedPath } from "react-router-dom"
import "./navbar.css"

export default function Navbar() {
    return (
    <nav className="nav">
        <Link to="/" className ="home">Home</Link>
        <ul>
            <CustomLink to="/hardware">Hardware</CustomLink>
            <CustomLink to="/animation">Animation</CustomLink>
            <CustomLink to="/software">Software</CustomLink>
            <CustomLink to="/cad">CAD</CustomLink>
            {/* <li><a href="#">Work</a></li>
            <li><a href="#contact">Contact</a></li> */}
        {/* &nbsp; */}
        </ul>
    </nav>
    );
}

function CustomLink ({to, children, ...props}) {
    const resolvedPath = useResolvedPath(to)
    const isActive = useMatch( {path: resolvedPath.pathname, end:true } )
    return (
        <li className={isActive ? "active" : ""}>
            <Link to={to} {...props}>
                {children}
            </Link>
        </li>
    )
}