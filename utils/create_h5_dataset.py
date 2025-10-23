import h5py
import numpy as np
import argparse
import os

def create_dataset(output_path: str, num_images: int = 100, img_size: int = 28):
    os.makedirs(os.path.dirname(output_path), exist_ok=True)
    images = np.random.randint(0, 256, (num_images, img_size, img_size), dtype=np.uint8)

    with h5py.File(output_path, "w") as f:
        f.create_dataset("images", data=images)

    print(f"✅ Dataset saved at {output_path} with {num_images} images of size {img_size}x{img_size}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--output", type=str, required=True, help="Output .h5 file path")
    parser.add_argument("--num_images", type=int, default=100)
    parser.add_argument("--img_size", type=int, default=28)
    args = parser.parse_args()

    create_dataset(args.output, args.num_images, args.img_size)
