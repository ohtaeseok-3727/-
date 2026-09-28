// Preview-only frame packing. Keep source artwork intact and preserve source-space pivots.
// Each isolated frame gets 64 source pixels of transparent padding on all sides.
globalThis.PaddedFrames = class {
  constructor(image, columns, rows, boundaries = []) {
    this.image = image;
    this.columns = columns;
    this.rows = rows;
    this.boundaries = boundaries;
    this.frames = new Map();
  }
  draw(ctx, col, row, nominalX, nominalY, dx, dy, scale) {
    const key = row * this.columns + col;
    let frame = this.frames.get(key);
    if (!frame) {
      const edges = this.boundaries[row] || Array.from({length:this.columns+1}, (_,i)=>Math.floor(i*this.image.width/this.columns));
      const x = edges[col], y = this.rows[row];
      const width = edges[col+1]-x, height = this.rows[row+1]-y, pad = 64;
      const canvas = document.createElement('canvas');
      canvas.width = width+pad*2; canvas.height = height+pad*2;
      const buffer = canvas.getContext('2d');
      buffer.imageSmoothingEnabled = false;
      buffer.drawImage(this.image,x,y,width,height,pad,pad,width,height);
      frame = {canvas,x:x-pad,y:y-pad};
      this.frames.set(key,frame);
    }
    ctx.drawImage(frame.canvas,dx+(frame.x-nominalX)*scale,dy+(frame.y-nominalY)*scale,frame.canvas.width*scale,frame.canvas.height*scale);
  }
};
