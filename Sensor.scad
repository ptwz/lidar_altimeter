module mirror_copy(v = [1, 0, 0]) {
    children();
    mirror(v) children();
}

module sensor(){
rotate([0,90,0])
union(){
translate([14.5,0,-2.5]) cylinder(h=40, d=19.5);
translate([-14.5,0,-2.5]) cylinder(h=40,d=19.5);
translate([0,0,12.45/2-2.5]) cube([48,20,12.45], center=true);
mirror_copy([0,1,0]) mirror_copy([1,0,0])
translate([48/2-9.5, -27.5/2, 0]) rotate([0,0,90]) translate([+4,0,0]) {
    cube([8,8,2.5],center=true);
    translate([-4,0,0]) cylinder(h=2.5, r=4, center=true);
    };
};
}