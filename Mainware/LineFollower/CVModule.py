"""
Module to view camera output and determine which way to turn for correction

Written w/ some help from chatGPT to reduce time reading CV documentation
"""

import cv2
import numpy as np


class CVModule:

    # setup camera, bounding box limits, line thickness
    def __init__(self):
        self.cap = cv2.VideoCapture(1, cv2.CAP_DSHOW)
        if not self.cap.isOpened():
            print("Error: Could not open camera.")
        else:
            print("cam opened")
        # working frame
        self.frame_x, self.frame_y, self.frame_w, self.frame_h = 100, 200, 300, 100
        # bounding box
        self.bound_x, self.bound_y, self.bound_w, self.bound_h = 200, 200, 100, 100
        # TODO: determine actual thickness in webcam feed
        self.line_size = 100

    # find and return x position of bounding box around black line
    # return -1 if no box found
    def detect_line_position(self, frame, draw_on_frame=None):
        # make black and white
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

        # scale gradients as 0 to 255
        _, thresh = cv2.threshold(gray, 50, 255, cv2.THRESH_BINARY_INV)

        # find countours to find black line
        contours, _ = cv2.findContours(
            thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE
        )

        for contour in contours:
            # check if contour is big enough
            if cv2.contourArea(contour) > self.line_size:
                # get box bounding contour
                x, y, w, h = cv2.boundingRect(contour)

                cv2.rectangle(draw_on_frame, (x, y), (x + w, y + h), (255, 0, 0), 2)

                return (x + (x + w)) / 2

        # line not found
        return -1

    # return left if line is left of box, right if line is right of box, center if line is in box, error otherwise
    def capture_and_process(self):
        # capture one frame
        ret, frame = self.cap.read()

        if not ret:
            print("failed to capture image")
            return

        # draw bounding box in frame
        cv2.rectangle(
            frame,
            (self.bound_x, self.bound_y),
            (self.bound_x + self.bound_w, self.bound_y + self.bound_h),
            (0, 255, 0),
            2,
        )

        # find black line
        # linePos = self.detect_line_position(frame)

        # Crop the frame to your desired ROI
        cropped = frame[
            self.frame_y : self.frame_y + self.frame_h,
            self.frame_x : self.frame_x + self.frame_w,
        ]
        # Process only the cropped frame
        linePos = self.detect_line_position(cropped)

        # check if line detected & compare to bounding box
        if linePos == -1:
            return "error"
        else:
            # line is left of box
            if linePos > self.bound_x:
                return "right"
            # line is right of box
            elif linePos > self.bound_x - self.bound_w:
                return "left"
            # line is within box
            else:
                return "center"

    # captures image, displays it, writes which way to go to correct
    def capture_and_process_tester(self):
        # capture one frame
        ret, frame = self.cap.read()

        if not ret:
            print("failed to capture image")
            return

        # draw bounding box in frame
        cv2.rectangle(
            frame,
            (self.bound_x, self.bound_y),
            (self.bound_x + self.bound_w, self.bound_y + self.bound_h),
            (0, 255, 0),
            2,
        )

        # find black line
        # linePos = self.detect_line_position(frame)

        # Crop the frame to your desired ROI
        cropped = frame[
            self.frame_y : self.frame_y + self.frame_h,
            self.frame_x : self.frame_x + self.frame_w,
        ]

        # Process only the cropped frame
        linePos = self.detect_line_position(cropped, draw_on_frame=cropped)

        # check if line detected & compare to bounding box
        if linePos != -1:
            # line is left of box
            if linePos > self.bound_x:
                cv2.putText(
                    cropped,
                    "Line is RIGHT of box :: " + str(linePos),
                    (10, 50),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    (0, 0, 255),
                    2,
                )
            # line is right of box
            elif linePos < self.bound_x - self.bound_w:
                cv2.putText(
                    cropped,
                    "Line is LEFT of box :: " + str(linePos),
                    (10, 50),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    (0, 0, 255),
                    2,
                )
            # line is within box
            else:
                cv2.putText(
                    cropped,
                    "Line is WITHIN box :: " + str(linePos),
                    (10, 50),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    0.5,
                    (0, 255, 0),
                    2,
                )

        # Show the frame with bounding box and result
        cv2.imshow("Processed Image", cropped)
        cv2.waitKey(1)


cam = CVModule()
while True:
    cam.capture_and_process_tester()


# for i in range(5):
#     cap = cv2.VideoCapture(i, cv2.CAP_DSHOW)
#     if cap.isOpened():
#         print(f"Camera {i} is available.")
#         ret, frame = cap.read()
#         if ret:
#             cv2.imshow(f"Camera {i}", frame)
#             cv2.waitKey(1000)
#         cap.release()
# cv2.destroyAllWindows()
