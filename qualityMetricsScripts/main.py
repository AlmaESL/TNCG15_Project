import cv2
from importedMetrics import compute_metrics

# load image
img = cv2.imread('test.png')

qualityMetrics = compute_metrics(img)

# print metrics
for metric, value in qualityMetrics.items():
    print(f"{metric}: {value}")