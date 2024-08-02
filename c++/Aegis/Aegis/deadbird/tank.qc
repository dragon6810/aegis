$scale 0.4
$modelname	"models\tank\tank.mdl"
$cd "C:\Users\theto\Documents\Github\aegis\External\models\tank"
$body tank "tank"
$cdtexture "C:\Users\theto\Documents\Github\aegis\External\models\tank"
$sequence idle "tank"    loop fps 30 ACT_IDLE 1

$hbox 0 "TankRoot" -7.90 -1.00 -13.88 219.16 1.00 -11.88 0.00 0.00 0.00

$hbox 0 "TankHead" -99.29 -134.65 -142.70 142.38 -130.62 -138.63 0.01 0.00 -45.03

$hbox 0 "Bone001" 220.66 -0.58 -1.47 224.66 3.42 2.53 -44.97 89.99 -179.98

$controller 0 "TankHead" ZR 0 360