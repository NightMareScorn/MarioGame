"use client";

import React, { useState } from "react";
import { motion, AnimatePresence } from "framer-motion";
import { ChevronLeft, ChevronRight, Play } from "lucide-react";
import { useRouter } from "next/navigation";

const LEVELS = [
  { id: "1-1", title: "World 1-1", image: "/background/background_mountain.png", targetCSV: "level_1_1.csv", bestScore: "080000", time: "--:--" },
  { id: "1-2", title: "World 1-2", image: "/background/background_tree.png", targetCSV: "level_1_1_hidden.csv", bestScore: "000000", time: "--:--" },
  { id: "1-3", title: "World 1-3", image: "/background/background_cloud.png", targetCSV: "level_1_2_first_half.csv", bestScore: "000000", time: "--:--" },
  { id: "1-4", title: "World 1-4", image: "/background/background_mountain.png", targetCSV: "level_1_2_hidden.csv", bestScore: "000000", time: "--:--" },
  { id: "1-5", title: "World 1-5", image: "/background/background_tree.png", targetCSV: "level_1_2_hidden_2.csv", bestScore: "000000", time: "--:--" },
  { id: "1-6", title: "World 1-6", image: "/background/background_cloud.png", targetCSV: "level_1_2_second_half.csv", bestScore: "000000", time: "--:--" },
  { id: "2-1", title: "World 2-1", image: "/background/background_mountain.png", targetCSV: "level_2_3.csv", bestScore: "000000", time: "--:--" },
  { id: "2-2", title: "World 2-2", image: "/background/background_tree.png", targetCSV: "level_2_4.csv", bestScore: "000000", time: "--:--" },
  { id: "2-3", title: "World 2-3", image: "/background/background_cloud.png", targetCSV: "level_2_3.csv", bestScore: "000000", time: "--:--" },
  { id: "2-4", title: "World 2-4", image: "/background/background_mountain.png", targetCSV: "level_2_4.csv", bestScore: "000000", time: "--:--" },
  { id: "2-5", title: "World 2-5", image: "/background/background_tree.png", targetCSV: "level_2_3.csv", bestScore: "000000", time: "--:--" },
  { id: "2-6", title: "World 2-6", image: "/background/background_cloud.png", targetCSV: "level_2_4.csv", bestScore: "000000", time: "--:--" },
];

export default function LevelSelectScene() {
  const router = useRouter();
  const [index, setIndex] = useState(0);

  const next = () => {
    if (index < LEVELS.length - 1) setIndex(index + 1);
  };
  const prev = () => {
    if (index > 0) setIndex(index - 1);
  };

  return (
    <main className="relative w-screen h-screen overflow-hidden flex flex-col items-center justify-center bg-[#E0E0E0]">
      {/* Background Pattern */}
      <div className="absolute inset-0 opacity-5 pointer-events-none" 
           style={{ backgroundImage: 'radial-gradient(#000 1px, transparent 0)', backgroundSize: '40px 40px' }} />

      {/* Header */}
      <div className="absolute top-10 left-10">
        <h2 className="text-gray-500 font-bold text-sm uppercase tracking-widest">
          Map Selection - Super UI World
        </h2>
      </div>

      {/* Back Button */}
      <button 
        onClick={() => router.push("/")}
        className="absolute top-10 right-10 flex items-center group active:scale-95 transition-all"
      >
        <div className="w-10 h-10 border-4 border-black flex items-center justify-center bg-white shadow-[4px_4px_0px_black] group-hover:bg-gray-50 transition-colors">
          <ChevronLeft size={24} strokeWidth={4} className="text-red-600" />
        </div>
        <span className="text-3xl font-bold ml-4 text-[#E52521] uppercase tracking-tighter" 
              style={{ filter: 'drop-shadow(2px 2px 0px black)' }}>
          BACK
        </span>
      </button>

      {/* Carousel Container */}
      <div className="relative w-full h-[550px] flex items-center justify-center">
        <AnimatePresence initial={false}>
          {LEVELS.map((level, i) => {
            const isCenter = i === index;
            const isLeft = i === index - 1;
            const isRight = i === index + 1;

            if (!isCenter && !isLeft && !isRight) return null;

            return (
              <motion.div
                key={level.id}
                initial={false}
                animate={{
                  x: isCenter ? 0 : isLeft ? -480 : 480,
                  scale: isCenter ? 1 : 0.75,
                  opacity: isCenter ? 1 : 0.3,
                  zIndex: isCenter ? 10 : 0,
                  filter: isCenter ? "blur(0px)" : "blur(4px)",
                }}
                transition={{ type: "spring", stiffness: 200, damping: 25 }}
                className="absolute"
              >
                <MapCard level={level} isSide={!isCenter} />
              </motion.div>
            );
          })}
        </AnimatePresence>

        {/* Carousel Navigation Arrows */}
        <div className="absolute inset-0 flex items-center justify-between px-20 z-20 pointer-events-none">
           <div className="w-16">
             <AnimatePresence>
               {index > 0 && (
                 <motion.button
                   key="prev-btn"
                   initial={{ opacity: 0, scale: 0.5 }}
                   animate={{ opacity: 1, scale: 1 }}
                   exit={{ opacity: 0, scale: 0.5 }}
                   onClick={prev}
                   className="pointer-events-auto w-16 h-16 bg-white border-4 border-black flex items-center justify-center hover:bg-gray-100 transition-all shadow-[6px_6px_0px_black] active:translate-x-1 active:translate-y-1 active:shadow-none"
                 >
                   <ChevronLeft size={40} strokeWidth={4} className="text-gray-600" />
                 </motion.button>
               )}
             </AnimatePresence>
           </div>

           <div className="w-16">
             <AnimatePresence>
               {index < LEVELS.length - 1 && (
                 <motion.button
                   key="next-btn"
                   initial={{ opacity: 0, scale: 0.5 }}
                   animate={{ opacity: 1, scale: 1 }}
                   exit={{ opacity: 0, scale: 0.5 }}
                   onClick={next}
                   className="pointer-events-auto w-16 h-16 bg-white border-4 border-black flex items-center justify-center hover:bg-gray-100 transition-all shadow-[6px_6px_0px_black] active:translate-x-1 active:translate-y-1 active:shadow-none"
                 >
                   <ChevronRight size={40} strokeWidth={4} className="text-gray-600" />
                 </motion.button>
               )}
             </AnimatePresence>
           </div>
        </div>
      </div>

      {/* Play Button */}
      <motion.div 
        animate={{ y: [0, -4, 0] }}
        transition={{ duration: 1.5, repeat: Infinity, ease: "easeInOut" }}
        className="mt-4"
      >
        <button
          onClick={() => {
            const level = LEVELS[index];
            console.log("Playing level:", level.title, "CSV:", level.targetCSV);
            
            // Communication bridge to C++ WebView2
            if ((window as any).chrome?.webview) {
              (window as any).chrome.webview.postMessage(level.targetCSV);
            } else {
              alert(`Browser Mode: Launching ${level.title}\nFile: ${level.targetCSV}\n(In-game, this will start the level automatically)`);
            }
          }}
          className="bg-[#E52521] btn-retro py-4 px-20 rounded-sm flex items-center justify-center gap-4 group hover:scale-105 active:scale-95 transition-all shadow-[8px_8px_0px_black]"
        >
          <Play className="fill-white" size={28} />
          <span className="text-4xl font-bold uppercase tracking-widest text-white text-retro">
            Play
          </span>
        </button>
      </motion.div>
    </main>
  );
}

function MapCard({ level, isSide = false }: { level: typeof LEVELS[0], isSide?: boolean }) {
  return (
    <div className="flex flex-col items-center select-none">
      <div className={`w-[600px] bg-white border-8 border-black p-1 rounded-sm shadow-[16px_16px_0px_black] transition-all`}>
        <div className="bg-[#E52521] text-white text-center py-3 font-bold text-4xl uppercase tracking-tighter border-b-8 border-black text-retro">
          {level.title}
        </div>
        <div className="relative aspect-video bg-[#5C94FC] border-b-8 border-black overflow-hidden">
          <img src={level.image} className="w-full h-full object-cover" />
          
          {/* Decorative Assets */}
          <div className="absolute inset-0 flex items-center justify-center opacity-40 pointer-events-none">
             <div className="flex gap-6">
               {Array.from({ length: 3 }).map((_, i) => (
                 <div key={i} className="w-14 h-14 bg-yellow-400 border-4 border-black flex items-center justify-center text-3xl font-bold shadow-[4px_4px_0px_black]">?</div>
               ))}
             </div>
          </div>
        </div>
        
        {/* Info Bar */}
        <div className="bg-white p-4 flex justify-between items-center">
           <div className="flex items-center gap-3">
             <TrophyIcon className="w-8 h-8" />
             <span className="text-lg font-bold uppercase tracking-tighter text-black">BEST SCORE: <span className="text-gray-500 font-mono">{level.bestScore}</span></span>
           </div>
           <div className="flex items-center gap-4">
             {/* SMALLER CLOCK ICON (reduced from w-10 to w-8) */}
             <ClockIcon className="w-8 h-8" />
             <span className="text-lg font-bold uppercase tracking-tighter text-black">TIME: <span className="text-gray-500 font-mono">{level.time}</span></span>
           </div>
        </div>
      </div>
    </div>
  );
}

const TrophyIcon = ({ className }: { className?: string }) => (
  <svg viewBox="0 0 16 16" className={className} fill="none" xmlns="http://www.w3.org/2000/svg">
    <path d="M3 2H13V4H12V5H11V8C11 9.65685 9.65685 11 8 11V12H10V14H6V12H8V11C6.34315 11 5 9.65685 5 8V5H4V4H3V2Z" fill="#E52521" />
    <path d="M2 4H3V7C3 8.10457 3.89543 9 5 9V10C3.34315 10 2 8.65685 2 7V4Z" fill="#E52521" />
    <path d="M14 4H13V7C13 8.10457 12.1046 9 11 9V10C12.6569 10 14 8.65685 14 7V4Z" fill="#E52521" />
  </svg>
);

const ClockIcon = ({ className }: { className?: string }) => (
  <svg viewBox="0 0 16 16" className={className} fill="none" xmlns="http://www.w3.org/2000/svg">
    <circle cx="8" cy="8" r="7" fill="#5C94FC" />
    <path d="M8 3V8H12" stroke="black" strokeWidth="2.5" strokeLinecap="square" />
    <rect x="7" y="1" width="2" height="1.5" fill="black" />
  </svg>
);
