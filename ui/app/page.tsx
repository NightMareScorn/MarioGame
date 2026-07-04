"use client";

import React, { useState, useEffect } from "react";
import { motion, AnimatePresence } from "framer-motion";
import { Play, Settings } from "lucide-react";
import { useRouter } from "next/navigation";
import { Sprite } from "./components/Sprite";

export default function MenuScene() {
  const router = useRouter();
  const [hasStarted, setHasStarted] = useState(false);

  useEffect(() => {
    const handleKeyDown = (e: KeyboardEvent) => {
      if (e.code === "Space") {
        setHasStarted(true);
      }
    };
    window.addEventListener("keydown", handleKeyDown);
    return () => window.removeEventListener("keydown", handleKeyDown);
  }, []);

  return (
    <main className="relative w-screen h-screen overflow-hidden flex flex-col items-center justify-center bg-[#5C94FC]">
      {/* Background Elements */}
      <div className="absolute inset-0 z-0">
        {/* Sky Clouds */}
        <motion.div
          className="absolute top-20 left-[10%]"
          animate={{ x: [0, 50, 0] }}
          transition={{ duration: 20, repeat: Infinity, ease: "easeInOut" }}
        >
          <Sprite src="/tileset.png" x={352} y={632} width={31} height={23} scale={3} />
        </motion.div>
        
        <motion.div
          className="absolute top-10 right-[10%]"
          animate={{ x: [0, -60, 0] }}
          transition={{ duration: 30, repeat: Infinity, ease: "easeInOut" }}
        >
          <Sprite src="/tileset.png" x={624} y={608} width={47} height={23} scale={4} />
        </motion.div>
        
        {/* Mid Level Clouds */}
        <motion.div
          className="absolute top-[45%] left-[5%]"
          animate={{ x: [0, 30, 0] }}
          transition={{ duration: 12, repeat: Infinity, ease: "easeInOut" }}
        >
          <Sprite src="/tileset.png" x={352} y={632} width={31} height={23} scale={2} className="opacity-40" />
        </motion.div>

        {/* Hill - Grounded */}
        <div className="absolute bottom-[0px] left-[-32px]">
           <Sprite src="/tileset.png" x={112} y={741} width={79} height={34} scale={2.5} origin="bottom left" />
        </div>

        {/* Pipe 1 - Grounded */}
        <div className="absolute bottom-[0px] left-[150px]">
           <Sprite src="/tileset.png" x={112} y={624} width={31} height={63} scale={2.5} origin="bottom left" />
        </div>

        {/* Bush - Middle */}
        <div className="absolute bottom-[32px] left-[45%] -translate-x-1/2">
           <Sprite src="/tileset.png" x={120} y={704} width={63} height={15} scale={3} origin="bottom left" />
        </div>

        {/* Stair - Near Hole (Moved further left by 1 block: right-31% to right-34%) */}
        <div className="absolute bottom-[32px] right-[34%] flex flex-col items-end">
           <div className="flex leading-none"><Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} origin="bottom left" /></div>
           <div className="flex leading-none">
              <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} origin="bottom left" />
              <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} origin="bottom left" />
           </div>
           <div className="flex leading-none">
              <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} origin="bottom left" />
              <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} origin="bottom left" />
              <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} origin="bottom left" />
           </div>
        </div>

        {/* Pipe 2 - Grounded */}
        <div className="absolute bottom-[0px] right-[8%]">
           <Sprite src="/tileset.png" x={112} y={624} width={31} height={63} scale={1.8} origin="bottom left" />
        </div>

        {/* Ground Blocks - Use flex-shrink-0 to prevent gaps */}
        <div className="absolute bottom-0 w-screen flex flex-col z-20 overflow-hidden">
           <div className="flex w-full">
             {Array.from({ length: 100 }).map((_, i) => (
               <div key={`g1-${i}`} className="flex-shrink-0">
                 <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} />
               </div>
             ))}
           </div>
           <div className="flex w-full">
             {Array.from({ length: 100 }).map((_, i) => (
               <div key={`g2-${i}`} className="flex-shrink-0">
                 <Sprite src="/tileset.png" x={0} y={16} width={16} height={16} scale={2} />
               </div>
             ))}
           </div>
        </div>
      </div>

      {/* Title Section */}
      <motion.div
        initial={{ y: -100, opacity: 0 }}
        animate={{ y: 0, opacity: 1 }}
        transition={{ type: "spring", damping: 12 }}
        className="z-10 flex flex-col items-center mb-12"
      >
        <h1 className="text-7xl font-bold text-[#E52521] text-retro tracking-wider uppercase italic">
          Super Mario
        </h1>
      </motion.div>

      {/* Interaction Layer */}
      <AnimatePresence mode="wait">
        {!hasStarted ? (
          <motion.div
            key="press-start"
            initial={{ opacity: 0 }}
            animate={{ opacity: 1 }}
            exit={{ opacity: 0 }}
            className="z-10"
          >
            <p className="text-2xl font-bold text-white text-retro animate-blink uppercase">
              Press Space to Start
            </p>
          </motion.div>
        ) : (
          <motion.div
            key="menu-buttons"
            initial={{ scale: 0.8, opacity: 0 }}
            animate={{ scale: 1, opacity: 1 }}
            className="z-10 flex flex-col gap-6 w-64"
          >
            <button
              onClick={() => router.push("/level-select")}
              className="bg-[#E52521] btn-retro py-4 px-8 rounded-lg flex items-center justify-center gap-3 group overflow-hidden relative"
            >
              <div className="absolute inset-0 bg-white opacity-0 group-hover:opacity-10 transition-opacity" />
              <Play className="fill-white" size={24} />
              <span className="text-2xl font-bold uppercase tracking-widest text-white text-retro">
                Play
              </span>
            </button>

            <button className="bg-[#BCBCBC] btn-retro py-4 px-8 rounded-lg flex items-center justify-center gap-3 group relative overflow-hidden">
              <div className="absolute inset-0 bg-white opacity-0 group-hover:opacity-10 transition-opacity" />
              <Settings className="text-black" size={24} />
              <span className="text-2xl font-bold uppercase tracking-widest text-black">
                Settings
              </span>
            </button>
          </motion.div>
        )}
      </AnimatePresence>
    </main>
  );
}
