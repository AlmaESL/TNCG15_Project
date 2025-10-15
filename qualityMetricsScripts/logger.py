import cv2
import numpy as np
import os
import datetime
from tabulate import tabulate
import matplotlib.pyplot as plt

# ------------------------- Define Directories ------------------------- #


DIFF_DIR = "ColorDiffMaps"

os.makedirs(DIFF_DIR, exist_ok=True)

# get res dir
def get_diff_dir():
    return DIFF_DIR




# ------------------------- Logging Functions ------------------------- #
def get_log_file_path(log_dir, device):
    timestamp = datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S")
    return os.path.join(log_dir, f"{device}_log_{timestamp}.txt")


def log_results(log_dir, device, std, avg_diff, max_diff, max_pos, niqe, brisque, paq2piq, nima, piqe):
    """Logs evaluation results with a timestamped filename - txt."""
    log_file = get_log_file_path(log_dir, device)

    with open(log_file, 'a', encoding="utf-8") as f:
        f.write("\n" + "="*60 + "\n")
        f.write(f"{device} Evaluation Results - {datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write("="*60 + "\n")

        # All values rounded off to 3 decimals for visibility
        table_data = [
            ["Metric", "Value"],
            ["Avg STD - Graininess", f"{std:.3f}"],
            ["Avg Color Diff", f"{avg_diff:.3f}"],
            ["Max Color Diff", f"{max_diff:.3f} (at {max_pos})"],
            ["NIQE", f"{niqe:.3f}"],
            ["PIQE", f"{piqe:.3f}"],
            ["BRISQUE", f"{brisque:.3f}"],
            ["NIMA", f"{nima:.3f}"],
            ["PAQ2PIQ", f"{paq2piq:.3f}"]
        ]

        f.write(tabulate(table_data, headers="firstrow", tablefmt='grid'))
        f.write("\n\n")
    
    


# ------------------------- Color Difference Map Saving ------------------------- #
def save_color_difference_maps(diff_maps, save_dir):
    """Saves color difference maps as images with color bars."""
    for i, diff_map in enumerate(diff_maps):
        plt.figure(figsize=(6, 6))
        
        # Use viridis color map
        plt.imshow(diff_map, cmap='viridis')
        plt.colorbar(label='Color Difference')
        plt.axis('off')

        save_path = os.path.join(save_dir, f"color_diff_{i}.png")
        plt.savefig(save_path, bbox_inches='tight', pad_inches=0.1, dpi=300)
        plt.close()

    print(f"Saved {len(diff_maps)} color difference maps to directory")