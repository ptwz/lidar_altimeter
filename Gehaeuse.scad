include <sensor.scad>;

module raindrop2D(thick, radius, centered = true){
	if(!centered){
		translate([radius, radius, 0]){
			cylinder(h = thick, r = radius);
			linear_extrude(height = thick)
				polygon(points=[[radius * -cos(20), radius * sin(20)],[radius * cos(20), radius * sin(20)],[0, radius * 3]], paths=[[0,1,2]]);
		}
	}else if(centered){
		cylinder(h = thick, r = radius);
		linear_extrude(height = thick)
			polygon(points=[[radius * -cos(20), radius * sin(20)],[radius * cos(20), radius * sin(20)],[0, radius * 3]], paths=[[0,1,2]]);
	}	
}//end of module raindrop2D

module raindrop3D(radius, centered = true){
	if(!centered){
		translate([radius, radius, radius]){
			sphere(radius);
			translate([0, 0, radius * sin(20)]) cylinder(h = radius*3, r1 = radius * cos(20), r2 = 0);
		}
	}else if(centered){
		sphere(radius);
		translate([0, 0, radius*sin(20)]) cylinder(h = radius*3, r1 = radius * cos(20), r2 = 0);
	}	
}//end of module raindrop3D

module lidar_gehaeuse(){
difference(){
	union(){
		difference() {
			scale([1,1,.7]) union(){ // Aussenhuelle
				raindrop3D(50, true);
				translate([-2,0,0]) rotate([90,0,90]) raindrop2D(1, 65);
			}
			translate([-27,0,0]){ // Unterseite offen
				cube([50, 200, 400], center=true);
			}
            difference(){
                scale([1,1,.7]) raindrop3D(48, true); // Innen hohl
                //Dome fuer LIDAR-Modul stehen lassen
                mirror_copy([0,1,0]) mirror_copy([0,0,1]) translate([37.5/2+2,14.5,13.75]) rotate([0,90,0]) cylinder(d=8,h=37.5-2,center=true);
                // Dome für Leiterplatte
                translate([0,0,40]) mirror_copy([0,1,0]) mirror_copy([0,0,1]) translate([25,15,15]) rotate([0,90,0]) cylinder(d=8,h=45,center=true);
                
            }
		}
		translate([40-1.25,0,2.5]){ // "Guckloch"-Flaeche
			rotate([0,90,0]) #cylinder(h=2.5, d=50, center=true);
		}
	}


	translate([40+10,0,0]){
		rotate([0,90,0]) cylinder(h=20,r=40, center=true);
	}
 translate([2.5,0,0]) rotate([90,0,0]) sensor();
}
}

//projection(cut=true) translate([0,0,-4]) rotate([0,-90,0]) 
 //projection(cut=true)  translate([0,0,-25])
lidar_gehaeuse();


