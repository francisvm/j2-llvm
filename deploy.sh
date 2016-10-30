set -e

if [ "$TRAVIS_PULL_REQUEST" != "false" -o "$TRAVIS_BRANCH" != "slides" ]; then
  echo "Skipping deploy."
  exit 0
fi

REPO=$(git config remote.origin.url)
SSH_REPO=${REPO/https:\/\/github.com\//git@github.com:}
SHA=$(git rev-parse --verify HEAD)

git clone $REPO out -b gh-pages
cp -r sources/* out/sources/
cp -r vendor/* out/vendor/
cp index.html out/
cd out
git config user.name "Francis Visoiu Mistrih"
git config user.email "visoiu_f@epita.fr"

if git diff --exit-code; then
  echo "No changes."
  exit 0
fi
git add --all
git commit -m "gh-pages: update: ${SHA}"

ENCRYPTED_KEY_VAR="encrypted_${ENCRYPTION_LABEL}_key"
ENCRYPTED_IV_VAR="encrypted_${ENCRYPTION_LABEL}_iv"
ENCRYPTED_KEY=${!ENCRYPTED_KEY_VAR}
ENCRYPTED_IV=${!ENCRYPTED_IV_VAR}
openssl aes-256-cbc -K $ENCRYPTED_KEY -iv $ENCRYPTED_IV -in ../id_rsa.enc -out id_rsa -d
chmod 600 id_rsa
eval `ssh-agent -s`
ssh-add id_rsa

git push $SSH_REPO gh-pages
