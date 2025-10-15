import cv2
from importedMetrics import compute_metrics
from spd import compute_spd
from scielab import scielab, opponent_to_lab, compute_color_difference
from logger import (
    save_color_difference_maps,
    get_diff_dir
)


SCREEN_WIDTH = 3000
SCREEN_HEIGHT = 2000
SCREEN_DIAGONAL = 14

spd = compute_spd(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_DIAGONAL)


# load image
img = cv2.imread('test.png')
img2 = cv2.imread('test2.png')

qualityMetrics = compute_metrics(img)

# print automatic metrics
for metric, value in qualityMetrics.items():
    print(f"{metric}: {value}")
   
# compute color differences using scielab
scielab_img = scielab(img, spd)
scielab_img2 = scielab(img2, spd)

scielab_lab = opponent_to_lab(scielab_img)
scielab_lab2 = opponent_to_lab(scielab_img2)

avg_diff, max_diff, max_pos, diff_map = compute_color_difference(scielab_lab, scielab_lab2)

print("avg color diff: ", avg_diff)

resDir = get_diff_dir()

# save color diff map 
save_color_difference_maps([diff_map], resDir)