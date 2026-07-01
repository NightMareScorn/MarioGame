import React from "react";

interface SpriteProps {
  src: string;
  x: number;
  y: number;
  width: number;
  height: number;
  scale?: number;
  className?: string;
  origin?: "top left" | "bottom left" | "center";
}

export const Sprite: React.FC<SpriteProps> = ({ 
  src, x, y, width, height, scale = 1, className = "", origin = "top left" 
}) => {
  return (
    <div
      className={className}
      style={{
        backgroundImage: `url(${src})`,
        backgroundPosition: `-${x}px -${y}px`,
        width: `${width}px`,
        height: `${height}px`,
        transform: `scale(${scale})`,
        transformOrigin: origin,
        imageRendering: "pixelated",
      }}
    />
  );
};
