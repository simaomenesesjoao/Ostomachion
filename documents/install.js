#!/usr/bin/env node
import fs from 'fs';
import path from 'path';
import { marked } from 'marked';

// Usage: node install.js <destination_root>
// node install.js /home/simao/projectos_pessoais/website/web/projects/ostomachion
// Run this script from the root of the source markdown folder

if (process.argv.length < 3) {
  console.error('Usage: node install.js <destination_root>');
  process.exit(1);
}

const sourceRoot = process.cwd(); // Current working directory as source root
const destRoot = path.resolve(process.argv[2]);

function extractMetadata(mdContent) {
  const match = mdContent.match(/^---\n([\s\S]*?)\n---\n/);
  if (!match) return { metadata: null, strippedContent: mdContent };

  const yaml = match[1];
  const strippedContent = mdContent.slice(match[0].length);
  const metadataObj = {};

  yaml.split('\n').forEach(line => {
    const [key, value] = line.split(/:\s*(.+)/); // match key: value
    try {
      metadataObj[key.trim()] = JSON.parse(value); // parse arrays, strings, etc.
    } catch {
      metadataObj[key.trim()] = value.trim();
    }
  });

  return {
    metadata: JSON.stringify(metadataObj, null, 2),
    strippedContent
  };
}

function renderLaTeX(html) {
  return html;
}

function convertMdToHtml(mdContent, title = '') {
  const { metadata, strippedContent } = extractMetadata(mdContent);

  const content = marked.parse(strippedContent);
  const htmlBody = renderLaTeX(content);

  const metadataScript = metadata
    ? `<script type="application/json" id="metadata">\n${metadata}\n</script>\n`
    : '';

  return `
${metadataScript}
${htmlBody}`;
}

function ensureDirExists(dirPath) {
  if (!fs.existsSync(dirPath)) {
    fs.mkdirSync(dirPath, { recursive: true });
  }
}

// List of image extensions to copy
const imageExtensions = ['.svg', '.gif', '.png', '.jpg', '.jpeg', '.webp'];

function processFolder(currentSrcFolder, currentDestFolder, relFolder) {
  ensureDirExists(currentDestFolder);

  const items = fs.readdirSync(currentSrcFolder, { withFileTypes: true });

  for (const item of items) {

    if (item.name === 'node_modules') {
      continue; // Skip node_modules directory
    }

    const srcPath  = path.join(currentSrcFolder, item.name);
    const destPath = path.join(currentDestFolder, item.name);
    const relPath  = path.join(relFolder, item.name);

    if (item.isDirectory() ) {
      processFolder(srcPath, destPath, relPath);
    } else if (item.isFile()) {
      if (item.name.endsWith('.md')) {
        let mdContent = fs.readFileSync(srcPath, 'utf-8');

        // Optional: fix relative image paths if needed here
        const imgPath = path.dirname(relPath);
        const htmlContent1 = convertMdToHtml(mdContent, item.name.replace('.md', ''));

        const htmlContent = htmlContent1.replace(
          /<img\s+([^>]*?)src="(?!https?:\/\/)([^"]+)"(.*?)>/g,
          (match, beforeSrc, srcValue, afterSrc) => {

            console.log(`regexed`);
            const newSrc = path.posix.join(imgPath, srcValue);
            console.log(`regexed newsrc: ${newSrc}`);
            return `<img ${beforeSrc}src="${newSrc}"${afterSrc}>`;
          }
        );

        const destHtmlPath = destPath.replace(/\.md$/, '.html');
        fs.writeFileSync(destHtmlPath, htmlContent);
        console.log(`Converted: ${srcPath} → ${destHtmlPath}`);
        console.log(`imgPath: ${imgPath}`);

      } else if (imageExtensions.includes(path.extname(item.name).toLowerCase())) {
        // Copy image files as-is
        fs.copyFileSync(srcPath, destPath);
        console.log(`Copied image: ${srcPath} → ${destPath}`);
      }
      // else: ignore other file types
    }
  }
}

// Run the script
processFolder(sourceRoot, path.join(destRoot, "Ostomachion"), path.join("assets", "projects", "Ostomachion"));
