import cv2
from glob import glob
from pathlib import Path

for i, file in enumerate(glob('assets/images/*.png')):
    path = Path(file)
    im = cv2.imread(file)
    h, w = im.shape[:2]
    new_w = 512
    new_h = int(h/w*new_w)
    new_im = cv2.resize(im, (new_w, new_h))
    print(new_im.shape)
    cv2.imwrite(path.with_name(f'{i}.png').as_posix(), new_im)
    path.unlink()